/*
 * Copyright (c) 2015-2019 Contributors as noted in the AUTHORS file
 *
 * This file is part of Solo5, a sandboxed execution environment.
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdarg.h>
#include <stddef.h>

#include "solo5.h"
#include "../../bindings/lib.c"
#include "../../bindings/printf.c"

#define NSEC_PER_SEC 1000000000ULL
#define COUNT 1000

__attribute__((__used__))
static int source_id = 0;

static void printf(const char *fmt, ...)
    __attribute__((format(printf, 1, 2)));

static void printf(const char *fmt, ...)
{
    char buffer[1024];
    va_list args;
    size_t size;

    va_start(args, fmt);
    size = vsnprintf(buffer, sizeof buffer, fmt, args);
    va_end(args);

    if (size >= sizeof buffer)
    {
        const char trunc[] = "(truncated)\n";
        solo5_console_write(buffer, sizeof buffer - 1);
        solo5_console_write(trunc, sizeof trunc - 1);
    }
    else
    {
        solo5_console_write(buffer, size);
    }
}

static void puts(const char *s)
{
    solo5_console_write(s, strlen(s));
}

__attribute__((__used__)) static void nop(void)
{
}

static uint64_t rdtsc(void)
{
    uint32_t lo, hi;
    __asm__ volatile("rdtsc"
                : "=a"(lo), "=d"(hi)
                :
                :);
    return (uint64_t)hi<<32 | lo;
}

static int call_trampoline(int n, uint64_t key, int target_id, uint64_t source_id)
{
    int r = 0;
    if (target_id >= 512)
        return r;
    n = n, key = key;
    __asm__ volatile("mov %0, %%edi;"
                     "movq %1, %%rsi;"
                     "mov %2, %%ecx;"
                     "movq %3, %%r11;"
                     :
                     : "r"(n), "r"(key), "r"(target_id), "r"(source_id)
                     :"edi","rsi","ecx","r11");
    __asm__ volatile("callq 0xfc000;"
                     "mov %%ebx, %0;"
                     : "=m"(r)
                     :
                     : "memory");
    return r;
}

int solo5_app_main(const struct solo5_start_info *si)
{
    (void)si;

    uint64_t source_rsp = 0;
    __asm__ volatile("mov %%rsp, %0;"
                     : "=m"(source_rsp)
                     :
                     : "memory");
    printf("rsp:0x%lx\n",source_rsp);

    int r = 0;
    solo5_time_t ta = 0, tb = 0;  
    ta = rdtsc();
    r = call_trampoline(6, 0x1000, 0, 4);
    tb = rdtsc();
    printf("FIRST CALL: ");
    if (r == 21)
        puts("FUNC SUCCESS\n");
    printf("TIME USE: %llu\n",
        (unsigned long long)(tb - ta));  

    unsigned long long sum_time = 0;
    for (int i = 0; i < COUNT; i++)
    {
       ta = rdtsc();
        r = call_trampoline(6, 0x1000, 0, 4);
        tb = rdtsc();
        printf("Call %d: ",i+1);
        if (r == 21)
           puts("FUNC SUCCESS\n");
         printf("TIME USE: %llu\n",
            (unsigned long long)(tb - ta));
        if (r == 1)
           puts("TRAMPOLINE SUCCESS\n");
        sum_time = sum_time + (unsigned long long)(tb-ta);
    }
    printf("AVERAGE TIME: %llu\n",sum_time/COUNT);

    __asm__ volatile("mov %%rsp, %0;"
                     : "=m"(source_rsp)
                     :
                     : "memory");
    printf("rsp:0x%lx\n",source_rsp);
    
    return SOLO5_EXIT_SUCCESS;
}
