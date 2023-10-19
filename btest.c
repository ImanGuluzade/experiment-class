
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <setjmp.h>
#include <math.h>
#include "btest.h"


float strtof(const char *nptr, char **endptr);

#define TIMEOUT_LIMIT 10

#define TEST_RANGE 500000


#define MAX_TEST_VALS 13*TEST_RANGE


extern test_rec test_set[]; 


static int grade = 0;


static int timeout_limit = TIMEOUT_LIMIT;

static char* test_fname = NULL;

static int has_arg[3] = {0,0,0};
static unsigned argval[3] = {0,0,0};

static int global_rating = 0;


typedef void handler_t(int);

handler_t *Signal(int signum, handler_t *handler) 
{
    struct sigaction action, old_action;

    action.sa_handler = handler;  
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_RESTART;

    if (sigaction(signum, &action, &old_action) < 0)
	perror("Signal error");
    return (old_action.sa_handler);
}


sigjmp_buf envbuf;
void timeout_handler(int sig) {
    siglongjmp(envbuf, 1);
}


static int random_val(int min, int max)
{
    double weight = rand()/(double) RAND_MAX;
    int result = min * (1-weight) + max * weight;
    return result;
}


static int gen_vals(int test_vals[], int min, int max, int test_range, int arg)
{
    int i;
    int test_count = 0;


    if (has_arg[arg]) {
	test_vals[0] = argval[arg];
	return 1;
    }

   
    if ((min == 1 && max == 1)) { 
	unsigned smallest_norm = 0x00800000;
	unsigned one = 0x3f800000;
	unsigned largest_norm = 0x7f000000;
	
	unsigned inf = 0x7f800000;
	unsigned nan =  0x7fc00000;
	unsigned sign = 0x80000000;

	
	if (test_range > (1 << 23)) {
	    test_range = 1 << 23;
	}
	
	

	for (i = 0; i < test_range; i++) {
	   
	    test_vals[test_count++] = i; 
	    test_vals[test_count++] = sign | i;
	    
	    
	    test_vals[test_count++] = smallest_norm + i;
	    test_vals[test_count++] = smallest_norm - i;
	    test_vals[test_count++] = sign | (smallest_norm + i);
	    test_vals[test_count++] = sign | (smallest_norm - i);
	    
	    
	    test_vals[test_count++] = one + i;
	    test_vals[test_count++] = one - i;
	    test_vals[test_count++] = sign | (one + i);
	    test_vals[test_count++] = sign | (one - i);
	    
	
	    test_vals[test_count++] = largest_norm - i; 
	    test_vals[test_count++] = sign | (largest_norm - i); 
	}
	

	test_vals[test_count++] = inf;
	test_vals[test_count++] = sign | inf;
	test_vals[test_count++] = nan;
	test_vals[test_count++] = sign | nan;

	return test_count;
    }



    if (max - MAX_TEST_VALS <= min) {
	for (i = min; i <= max; i++)
	    test_vals[test_count++] = i;
	return test_count;
    }

    
    for (i = 0; i < test_range; i++) {

	
	test_vals[test_count++] = min + i;
	test_vals[test_count++] = max - i;

	
	if (i >= min && i <= max)
	    test_vals[test_count++] = i;
	if (-i >= min && -i <= max)
	    test_vals[test_count++] = -i;


	test_vals[test_count++] = random_val(min, max);

    }
    return test_count;
}


static int test_0_arg(funct_t f, funct_t ft, char *name)
{
    int r = f();
    int rt = ft();
    int error =  (r != rt);

    if (error && !grade)
	printf("ERROR: Test %s() failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n", name, r, r, rt, rt);

    return error;
}


static int test_1_arg(funct_t f, funct_t ft, int arg1, char *name)
{
    funct1_t f1 = (funct1_t) f;
    funct1_t f1t = (funct1_t) ft;
    int r, rt, error;

    r = f1(arg1);
    rt = f1t(arg1);
    error = (r != rt);
    if (error && !grade)
	printf("ERROR: Test %s(%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n", name, arg1, arg1, r, r, rt, rt);

    return error;
}


static int test_2_arg(funct_t f, funct_t ft, int arg1, int arg2, char *name)
{
    funct2_t f2 = (funct2_t) f;
    funct2_t f2t = (funct2_t) ft;
    int r = f2(arg1, arg2);
    int rt = f2t(arg1, arg2);
    int error = (r != rt);

    if (error && !grade)
	printf("ERROR: Test %s(%d[0x%x],%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n", name, arg1, arg1, arg2, arg2, r, r, rt, rt);

    return error;
}


static int test_3_arg(funct_t f, funct_t ft, 
		      int arg1, int arg2, int arg3, char *name)
{
    funct3_t f3 = (funct3_t) f;
    funct3_t f3t = (funct3_t) ft;
    int r = f3(arg1, arg2, arg3);
    int rt = f3t(arg1, arg2, arg3);
    int error = (r != rt);

    if (error && !grade)
	printf("ERROR: Test %s(%d[0x%x],%d[0x%x],%d[0x%x]) failed...\n...Gives %d[0x%x]. Should be %d[0x%x]\n", name, arg1, arg1, arg2, arg2, arg3, arg3, r, r, rt, rt);

    return error;
}


static int test_function(test_ptr t) {
    int test_counts[3];
    int args = t->args;
    int arg_test_range[3];
    int i, a1, a2, a3;        
    int errors = 0;

    
    static int arg_test_vals[3][MAX_TEST_VALS]; 

    
    if (args < 0 || args > 3) {
	printf("Configuration error: invalid number of args (%d) for function %s\n", args, t->name);
	exit(1);
    }

   
    if (args == 1) {
	arg_test_range[0] = TEST_RANGE;
    }
    else if (args == 2) {
	arg_test_range[0] = pow((double)TEST_RANGE, 0.5);
	arg_test_range[1] = arg_test_range[0];
    }
    else {
	arg_test_range[0] = pow((double)TEST_RANGE, 0.333);
	arg_test_range[1] = arg_test_range[0];
	arg_test_range[2] = arg_test_range[0];
    }

    
    if (arg_test_range[0] < 1)
	arg_test_range[0] = 1;
    if (arg_test_range[1] < 1) 
	arg_test_range[1] = 1;
    if (arg_test_range[2] < 1) 
	arg_test_range[2] = 1;

    
    for (i = 0; i < args; i++) {
	test_counts[i] =  gen_vals(arg_test_vals[i], 
				   t->arg_ranges[i][0],
				   t->arg_ranges[i][1],
				   arg_test_range[i],   
				   i);

    }

    
    if (timeout_limit > 0) {
	int rc;
	rc = sigsetjmp(envbuf, 1);
	if (rc) {
	
	    errors = 1;
	    printf("ERROR: Test %s failed.\n  Timed out after %d secs (probably infinite loop)\n", t->name, timeout_limit);
	    return errors;
	}
	alarm(timeout_limit);
    }


    
    if (args == 0) {
	errors += test_0_arg(t->solution_funct, t->test_funct, t->name);
	return errors;
    } 

    
    for (a1 = 0; a1 < test_counts[0]; a1++) {
	if (args == 1) {
	    errors += test_1_arg(t->solution_funct, 
				 t->test_funct,
				 arg_test_vals[0][a1],
				 t->name);

	
	    if (errors)
		return errors;
	} 
	else {
	    
	    for (a2 = 0; a2 < test_counts[1]; a2++) {
		if (args == 2) {
		    errors += test_2_arg(t->solution_funct, 
					 t->test_funct,
					 arg_test_vals[0][a1], 
					 arg_test_vals[1][a2],
					 t->name);

		  
		    if (errors)
			return errors;
		} 
		else {
		
		    for (a3 = 0; a3 < test_counts[2]; a3++) {
			errors += test_3_arg(t->solution_funct, 
					     t->test_funct,
					     arg_test_vals[0][a1], 
					     arg_test_vals[1][a2],
					     arg_test_vals[2][a3],
					     t->name);
			
		
			if (errors)
			    return errors;
		    }
		}
	    }
	}
    }

    
    return errors;
}

 
static int run_tests() 
{
    int i;
    int errors = 0;
    double points = 0.0;
    double max_points = 0.0;

    printf("Score\tRating\tErrors\tFunction\n");

    for (i = 0; test_set[i].solution_funct; i++) {
	int terrors;
	double tscore;
	double tpoints;
	if (!test_fname || strcmp(test_set[i].name,test_fname) == 0) {
	    int rating = global_rating ? global_rating : test_set[i].rating;
	    terrors = test_function(&test_set[i]);
	    errors += terrors;
	    tscore = terrors == 0 ? 1.0 : 0.0;
	    tpoints = rating * tscore;
	    points += tpoints;
	    max_points += rating;

	    if (grade || terrors < 1)
		printf(" %.0f\t%d\t%d\t%s\n", 
		       tpoints, rating, terrors, test_set[i].name);

	}
    }

    printf("Total points: %.0f/%.0f\n", points, max_points);
    return errors;
}

static int get_num_val(char *sval, unsigned *valp) {
    char *endp;


    int ishex = 0;
    int isfloat = 0;
    int i;
    for (i = 0; sval[i]; i++) {
	switch (sval[i]) {
	case 'x':
	case 'X':
	    ishex = 1;
	    break;
	case 'e':
	case 'E':
	    if (!ishex)
		isfloat = 1;
	    break;
	case '.':
	    isfloat = 1;
	    break;
	default:
	    break;
	}
    }
    if (isfloat) {
	float fval = strtof(sval, &endp);
	if (!*endp) {
	    *valp = *(unsigned *) &fval;
	    return 1;
	}
	return 0;
    } else {
	long long int llval = strtoll(sval, &endp, 0);
	long long int upperbits = llval >> 31;
	
	if (!*valp && (upperbits == 0 || upperbits == -1 || upperbits == 1)) {
	    *valp = (unsigned) llval;
	    return 1;
	}
	return 0;
    }
}


static void usage(char *cmd) {
    printf("Usage: %s [-hg] [-r <n>] [-f <name> [-1|-2|-3 <val>]*] [-T <time limit>]\n", cmd);
    printf("  -1 <val>  Specify first function argument\n");
    printf("  -2 <val>  Specify second function argument\n");
    printf("  -3 <val>  Specify third function argument\n");
    printf("  -f <name> Test only the named function\n");
    printf("  -g        Compact output for grading (with no error msgs)\n");
    printf("  -h        Print this message\n");
    printf("  -r <n>    Give uniform weight of n for all problems\n");
    printf("  -T <lim>  Set timeout limit to lim\n");
    exit(1);
}



int main(int argc, char *argv[])
{
    int errors;
    char c;

    
    while ((c = getopt(argc, argv, "hgf:r:T:1:2:3:")) != -1)
        switch (c) {
        case 'h':
	    usage(argv[0]);
	    break;
	case 'g':
	    grade = 1;
	    break;
	case 'f':
	    test_fname = strdup(optarg);
	    break;
	case 'r':
	    global_rating = atoi(optarg);
	    if (global_rating < 0)
		usage(argv[0]);
	    break;
	case '1':
	    has_arg[0] = get_num_val(optarg, &argval[0]);
	    if (!has_arg[0]) {
		printf("Bad argument '%s'\n", optarg);
		exit(0);
	    }
	    break;
	case '2':
	    has_arg[1] = get_num_val(optarg, &argval[1]);
	    if (!has_arg[1]) {
		printf("Bad argument '%s'\n", optarg);
		exit(0);
	    }
	    break;
	case '3':
	    has_arg[2] = get_num_val(optarg, &argval[2]);
	    if (!has_arg[2]) {
		printf("Bad argument '%s'\n", optarg);
		exit(0);
	    }
	    break;
	case 'T':
	    timeout_limit = atoi(optarg);
	    break;
	default:
	    usage(argv[0]);
	}

    if (timeout_limit > 0) {
	Signal(SIGALRM, timeout_handler);
    }


    errors = run_tests();

    return 0;
}
