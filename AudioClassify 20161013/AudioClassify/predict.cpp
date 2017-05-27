#include "stdafx.h"
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "linear.h"

int print_null(const char *s,...) {return 0;}

static int (*info)(const char *fmt,...) = &printf;

struct feature_node *x;
int max_nr_attr = 64;

struct model* model_;
int flag_predict_probability=0;

void exit_input_error(int line_num)
{
	fprintf(stderr,"Wrong input format at line %d\n", line_num);
	exit(1);
}

static char *line = NULL;
static int max_line_len;

static char* readline(FILE *input)
{
	int len;

	if(fgets(line,max_line_len,input) == NULL)
		return NULL;

	while(strrchr(line,'\n') == NULL)
	{
		max_line_len *= 2;
		line = (char *) realloc(line,max_line_len);
		len = (int) strlen(line);
		if(fgets(line+len,max_line_len-len,input) == NULL)
			break;
	}
	return line;
}

void do_predict(MatrixXf features, int* predict_l)
{
	int nr_class = get_nr_class(model_);
	double *prob_estimates = NULL;
	int nr_feature = get_nr_feature(model_);
	int n = nr_feature;

	if (flag_predict_probability)
	{
		int *labels;

		if (!check_probability_model(model_))
		{
			fprintf(stderr, "probability output is only supported for logistic regression\n");
			exit(1);
		}

		labels = (int *)malloc(nr_class*sizeof(int));
		get_labels(model_, labels);
		prob_estimates = (double *)malloc(nr_class*sizeof(double));
		free(labels);
	}

	max_line_len = 1024;
	line = (char *)malloc(max_line_len*sizeof(char));
	int k = 0;
	for (int col = 0; col < features.cols(); col++)
	{
		x = (struct feature_node *)malloc(sizeof(feature_node)*(features.rows()+1));
		int i = 0;
		int inst_max_index = 0; // strtol gives 0 if wrong format
		for (i = 0; i < features.rows(); i++)
		{
			if (i >= max_nr_attr - 2)	// need one more for index = -1
			{
				max_nr_attr *= 2;
				x = (struct feature_node *) realloc(x, max_nr_attr*sizeof(struct feature_node));
			}
			x[i].index = i+1;
			x[i].value = features(i, col);
		}
		x[i].index = -1;
		predict_l[k] = predict(model_, x);
		k++;

	}
}

void exit_with_help()
{
	printf(
	"Usage: predict [options] test_file model_file output_file\n"
	"options:\n"
	"-b probability_estimates: whether to output probability estimates, 0 or 1 (default 0); currently for logistic regression only\n"
	"-q : quiet mode (no outputs)\n"
	);
	exit(1);
}

int model_predict(MatrixXf features,FILE *fp_ml,int *predict_label)
{

	if((model_=load_model(fp_ml))==0)
	{
		fprintf(stderr,"can't open model file \n");
		exit(1);
	}
	do_predict(features, predict_label);
	free_and_destroy_model(&model_);
	free(line);
	return 0;
}

