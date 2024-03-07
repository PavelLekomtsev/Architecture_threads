#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "TinyExpr/tinyexpr.h"
#include <sys/time.h>

char expression[100];
FILE *f_write = fopen("result.txt", "w");

double res = 0;
pthread_mutex_t mutex;

double f(double x){
    te_variable vars[] = { "x", &x };
    int err;
    te_expr* expr = te_compile(expression, vars, 1, &err);
    double result = te_eval(expr);
    te_free(expr);
    return result;
}

double integrate(double from, double to, double step) { //thread - body
    double sum = 0;
    double x; 
    for (x = from; x < to - 1E-100*to; x+= step) 
    sum += f(x)*step;
    return sum;
}

struct IntegrateTask { 
    double from, to, step, res;
    int i_thread; 
};
 
void * integrateThread(void * data) { 
    struct IntegrateTask* task = (struct IntegrateTask *)data; 
    task->res=integrate(task->from, task->to, task->step); 

    pthread_mutex_lock(&mutex);
    res += task->res;
    pthread_mutex_unlock(&mutex);

    fprintf(f_write, "Result of thread №%d: %lf; borders: [%lf; %lf]\n", task->i_thread, task->res, task->from, task->to);

    pthread_exit(NULL);
}

inline double my_clock(void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}


int main() {

    FILE *f_read = fopen("expression.txt", "r");
    pthread_mutex_init(&mutex, NULL);

    for (int NUM_THREADS=1; NUM_THREADS<=16; NUM_THREADS*=2){
        
        double from, to;
        fprintf(f_write, "THREADS: %d\n\n", NUM_THREADS);
        fscanf(f_read, "%s\n%lf %lf", expression, &from, &to);
    
        pthread_t threads[NUM_THREADS]; 
        struct IntegrateTask tasks[NUM_THREADS]; 

        for (double step=1; step >= 1e-6; step/=10){
            fprintf(f_write, "Reading from file\n");
            fprintf(f_write, "Function: %s\n", expression);
            fprintf(f_write, "StartX: %lf, EndX: %lf, Step: %lf\n", from, to, step);
            struct IntegrateTask mainTask = {from, to, step};
            double distance = (mainTask.to - mainTask.from) / NUM_THREADS; 
            int i;
        
            double start_time, end_time;
            start_time = my_clock();

            for (i=0; i < NUM_THREADS; ++i) { 
                tasks[i].from = mainTask.from + i*distance; 
                tasks[i].to = mainTask.from + (i+1)*distance; 
                tasks[i].step = mainTask.step; 
                tasks[i].i_thread = i;

                pthread_create(&threads[i], NULL, integrateThread, (void*)&tasks[i]); 
            }
        
            
            for (i=0; i < NUM_THREADS; ++i) {
                pthread_join(threads[i], NULL);
                // res += tasks[i].res; 
            }

            end_time = my_clock();
            fprintf(f_write, "RESULT = %lf \n",res);
            fprintf(f_write, "Time is %lf seconds\n\n", end_time-start_time);
            res = 0;
        }
    }

    pthread_mutex_destroy(&mutex);
    fclose(f_read);
    fclose(f_write);
    return 0;
}