#ifndef SUBMIT_TO_SLURM
#define SUBMIT_TO_SLURM

#include <mutex>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <condition_variable>
#include <chrono>
#include <time.h>

mutex mtx;
clock_t start, start2;
clock_t finish, finish2;
char buf[8192];
bool fldone = false;
bool mcdone = false;
condition_variable cv;

void submit_fl(char* s1, const char* s2);
void check_finish();
double allend();

void submit_mc(char* s1, const char* s2);
void check_finish2();
double allend2();

double run_fl(char* s1, char* s2);
double run_mc(char* s1, char* s2);