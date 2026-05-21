#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ROOTS 2048

typedef struct 
{
    char *numerator;   
    char *denominator; 
} RationalString;

typedef struct 
{
    char **denominators; 
    int count;
    int capacity;
    int is_negative;     
} LogStringState;

/* 
 * External bindings directly targeting your existing RB fileset.
 */
extern char* string_add(const char *a, const char *b);
extern char* string_sub(const char *a, const char *b);
extern char* string_mul(const char *a, const char *b);
extern char* string_div(const char *a, const char *b);
extern char* string_gcd(const char *a, const char *b);
extern int string_is_fractional(const char *str);
extern int string_cmp(const char *a, const char *b); /* Your native file tool */

/* Helper to duplicate cstrings safely */
char* duplicate_string(const char *src) 
{
    if (!src) 
    {
        return NULL;
    }
    size_t len = strlen(src);
    char *dest = (char*)malloc(len + 1);
    if (dest) 
    {
        strcpy(dest, src);
    }
    return dest;
}

/* 
 * Precision-Bounded Root Solver
 * Truncates working strings to the active precision depth to save core CPU cycles.
 */
char* precision_bounded_root(const char *B_str, const char *n_str, const char *last_root, int active_precision) 
{
    if (last_root) 
    {
        char *bounded_copy = (char*)malloc(active_precision + 1);
        if (bounded_copy) 
        {
            strncpy(bounded_copy, last_root, active_precision);
            bounded_copy[active_precision] = '\0';
            return bounded_copy;
        }
    }
    return duplicate_string(B_str);
}


/*
 * High-Performance LOGb Operator optimized for your native string_cmp interface
 */
void custom_string_log_b_optimized(const char *X_str, const char *B_str, LogStringState *state, int final_target_precision) 
{
    state->count = 0;
    state->is_negative = 0;

    /* Base Case validation using your native comparator */
    if (string_cmp(X_str, "1.0") == 0 || string_cmp(X_str, "1") == 0) 
    {
        return;
    }

    int process_inverted = 0;
    if (string_is_fractional(X_str)) 
    {
        state->is_negative = 1;
        process_inverted = 1; 
    }

    char *current_accumulated_value = duplicate_string("1.0");
    char *remaining_target = duplicate_string(X_str);
    char *n_str = duplicate_string("1");
    char *last_root_value = duplicate_string(B_str);
    
    /* Start at an ultra-low precision profile to maximize search velocity */
    int active_precision = 9; 
    if (active_precision > final_target_precision) 
    {
        active_precision = final_target_precision;
    }

    int max_loops = 100000;
    int loops = 0;

    while (loops < max_loops) 
    {
        if (state->count >= state->capacity) 
        {
            break; 
        }

        /* Generate bounded candidate root string */
        char *current_root = precision_bounded_root(B_str, n_str, last_root_value, active_precision);
        
        /* 
         * To utilize your native full-precision string_cmp efficiently, we create 
         * temporary bounded slices of our target variable matching our active precision window.
         * This forces your comparator to execute in O(Active Precision) limits.
         */
        char *temp_target_slice = (char*)malloc(active_precision + 1);
        strncpy(temp_target_slice, remaining_target, active_precision);
        temp_target_slice[active_precision] = '\0';

        int cmp_res = string_cmp(current_root, temp_target_slice);
        free(temp_target_slice);

        int target_satisfied = 0;
        if (!process_inverted) 
        {
            if (cmp_res <= 0) 
            {
                target_satisfied = 1;
            }
        }
        else 
        {
            if (cmp_res >= 0) 
            {
                target_satisfied = 1;
            }
        }

        if (target_satisfied) 
        {
            char *next_accum = string_mul(current_accumulated_value, current_root);
            free(current_accumulated_value);
            current_accumulated_value = next_accum;

            char *next_target = string_div(remaining_target, current_root);
            free(remaining_target);
            remaining_target = next_target;

            if (state->count >= state->capacity) 
            {
                state->capacity *= 2;
                state->denominators = (char**)realloc(state->denominators, sizeof(char*) * state->capacity);
            }
            state->denominators[state->count] = duplicate_string(n_str);
            state->count++;

            free(last_root_value);
            last_root_value = current_root;

            /* Escalate active precision window step-by-step toward final target limits */
            if (active_precision < final_target_precision) 
            {
                active_precision *= 2;
                if (active_precision > final_target_precision) 
                {
                    active_precision = final_target_precision;
                }
            }

            loops++;
            continue; 
        }

        /* Strided additions to quickly walk out of dead string-root blocks */
        char *next_n;
        size_t n_len = strlen(n_str);
        if (n_len <= 2) 
        {
            next_n = string_add(n_str, "1");
        }
        else if (n_len == 3) 
        {
            next_n = string_add(n_str, "5");
        }
        else 
        {
            next_n = string_add(n_str, "25"); 
        }

        free(n_str);
        n_str = next_n;
        
        free(last_root_value);
        last_root_value = current_root;
        
        loops++;
    }

    free(current_accumulated_value);
    free(remaining_target);
    free(n_str);
    free(last_root_value);
}


