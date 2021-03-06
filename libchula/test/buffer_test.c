/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/* All files in libchula are Copyright (C) 2014 Alvaro Lopez Ortega.
 *
 *   Authors:
 *     * Alvaro Lopez Ortega <alvaro@gnu.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <check.h>
#include "libchula/buffer.h"

#define check_add(suit,func)                             \
    TCase *testcase_ ## func = tcase_create(#func);      \
    suite_add_tcase (suit, testcase_ ## func);           \
    tcase_add_test (testcase_ ##func, func);

#define run_test(suit)                          \
    SRunner *sr = srunner_create(suit);         \
    srunner_run_all(sr, CK_VERBOSE);            \
    return srunner_ntests_failed(sr);


START_TEST (init_heap)
{
    chula_buffer_t buf;

    /* Init */
    chula_buffer_init (&buf);
    ck_assert (buf.len == 0);
    ck_assert (buf.size == 0);
    ck_assert (buf.buf == NULL);

    /* Not empty */
    chula_buffer_add_str (&buf, "test");
    ck_assert (buf.len > 0);
    ck_assert (buf.buf != NULL);

    /* Empty */
    chula_buffer_clean (&buf);
    ck_assert (buf.len == 0);
    ck_assert (buf.buf != NULL);

    /* Free memory */
    chula_buffer_mrproper (&buf);
    ck_assert (buf.buf == NULL);
}
END_TEST


START_TEST (init_ptr)
{
    chula_buffer_t *buf = NULL;

    /* Instance */
    chula_buffer_new (&buf);
    ck_assert (buf != NULL);

    /* Emptiness */
    ck_assert (buf->len == 0);
    ck_assert (buf->buf == NULL);

    /* Not empty */
    chula_buffer_add_str (buf, "test");
    ck_assert (buf->len > 0);
    ck_assert (buf->buf != NULL);

    /* Empty */
    chula_buffer_clean (buf);
    ck_assert (buf->len == 0);
    ck_assert (buf->buf != NULL);
}
END_TEST

START_TEST (dup)
{
    ret_t           ret;
    chula_buffer_t  b1   = CHULA_BUF_INIT;
    chula_buffer_t *b2   = NULL;

    chula_buffer_add_str (&b1, "testing");
    ret = chula_buffer_dup (&b1, &b2);

    ck_assert (ret == ret_ok);
    ck_assert (&b1 != b2);
    ck_assert (b1.len == b2->len);
    ck_assert (b1.buf != b2->buf);
}
END_TEST

START_TEST (add)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add (&b, "123", 3);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 3);

    ret = chula_buffer_add (&b, "abcd", 4);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 3+4);

    ret = chula_buffer_add (&b, "", 0);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 3+4);
}
END_TEST

START_TEST (slice)
{
    ret_t           ret;
    chula_buffer_t  a    = CHULA_BUF_INIT;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    chula_buffer_add_str (&a, "0123456789");

    /* [2:4] */
    ret = chula_buffer_add_buffer_slice (&b, &a, 2, 4);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 2);
    ck_assert_str_eq (b.buf, "23");

    /* [2:] */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_buffer_slice (&b, &a, 2, CHULA_BUF_SLIDE_NONE);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 8);
    ck_assert_str_eq (b.buf, "23456789");

    /* [-2:] */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_buffer_slice (&b, &a, -2, CHULA_BUF_SLIDE_NONE);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 2);
    ck_assert_str_eq (b.buf, "89");

    /* [:4] */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_buffer_slice (&b, &a, CHULA_BUF_SLIDE_NONE, 4);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 4);
    ck_assert_str_eq (b.buf, "0123");

    /* [:-4] */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_buffer_slice (&b, &a, CHULA_BUF_SLIDE_NONE, -4);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 6);
    ck_assert_str_eq (b.buf, "012345");

    /* [:] */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_buffer_slice (&b, &a, CHULA_BUF_SLIDE_NONE, CHULA_BUF_SLIDE_NONE);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 10);
    ck_assert_str_eq (b.buf, "0123456789");

    /* [0:0] */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_buffer_slice (&b, &a, 0, 0);
    ck_assert (ret == ret_ok);
     ck_assert (b.len == 0);
}
END_TEST

START_TEST (add_fsize)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_fsize (&b, 0);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "0");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 900);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "900");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 1025);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "1.0K");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 2048);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "2.0K");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, (2*1024) + 300);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "2.3K");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 1024*1024);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "1.0M");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 9.5*1024*1024);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "9.5M");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 2.1 * 1024*1024*1024);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "2.1G");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 9.9 * 1024*1024*1024*1024);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "9.9T");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_fsize (&b, 6.2 * 1024*1024*1024*1024*1024);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "6.2P");
}
END_TEST

START_TEST (add_long10)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_long10 (&b, 1l);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "1");

    /* LONG_MIN: -2^31+1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_long10 (&b, -2147483647);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "-2147483647");

    /* LONG_MAX: 2^31-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_long10 (&b, 2147483647);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "2147483647");

    /* ULONG_MAX: 2^32-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_ulong10 (&b, 4294967295);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "4294967295");
}
END_TEST

START_TEST (add_llong10)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_llong10 (&b, 0);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "0");

    /* LLONG_MIN: -2^63+1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_llong10 (&b, -9223372036854775807);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "-9223372036854775807");

    /* LLONG_MAX: 2^63-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_llong10 (&b, 9223372036854775807);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "9223372036854775807");

    /* ULLONG_MAX: 2^64-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_ullong10 (&b, 18446744073709551615ull);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "18446744073709551615");
}
END_TEST

START_TEST (add_long16)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_ulong16 (&b, 1);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "1");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_ulong16 (&b, 10);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "a");

    /* LONG_MAX: 2^31-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_ulong16 (&b, 2147483647);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "7fffffff");

    /* ULONG_MAX: 2^32-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_ulong16 (&b, 4294967295);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "ffffffff");
}
END_TEST

START_TEST (add_llong16)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_ullong16 (&b, 0);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "0");

    /* LLONG_MAX: 2^63-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_ullong16 (&b, 9223372036854775807ull);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "7fffffffffffffff");

    /* ULLONG_MAX: 2^64-1 */
    chula_buffer_clean (&b);
    ret = chula_buffer_add_ullong16 (&b, 18446744073709551615ull);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "ffffffffffffffff");
}
END_TEST

START_TEST (add_va)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_va (&b, "");
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);

    chula_buffer_clean (&b);
    ret = chula_buffer_add_va (&b, "hi there");
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "hi there");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_va (&b, "%d is not one", 0);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "0 is not one");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_va (&b, "%03d rocks", 7);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "007 rocks");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_va (&b, "%s %%s", "tricky");
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "tricky %s");

    chula_buffer_clean (&b);
    ret = chula_buffer_add_va (&b, "%d - %1.1f - %s", 1, 2.1, "foo");
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "1 - 2.1 - foo");
}
END_TEST

START_TEST (add_char)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_char (&b, 'a');
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 1);
    ck_assert_str_eq (b.buf, "a");

    ret = chula_buffer_add_char (&b, 'b');
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 2);
    ck_assert_str_eq (b.buf, "ab");
}
END_TEST

START_TEST (add_char_n)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_add_char_n (&b, 'a', -1);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);
    ck_assert (b.buf == NULL);

    ret = chula_buffer_add_char_n (&b, 'a', 0);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);
    ck_assert (b.buf == NULL);

    ret = chula_buffer_add_char_n (&b, 'a', 5);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 5);
    ck_assert_str_eq (b.buf, "aaaaa");
}
END_TEST

START_TEST (prepend)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_prepend (&b, "", 0);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);
    ck_assert (b.buf == NULL);

    ret = chula_buffer_prepend (&b, "!", 1);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 1);
    ck_assert_str_eq (b.buf, "!");

    ret = chula_buffer_prepend (&b, " there", 6);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 7);
    ck_assert_str_eq (b.buf, " there!");

    ret = chula_buffer_prepend (&b, "Hi", 2);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 9);
    ck_assert_str_eq (b.buf, "Hi there!");

    chula_buffer_clean (&b);
    ret = chula_buffer_prepend (&b, "Allocated", 9);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 9);
    ck_assert_str_eq (b.buf, "Allocated");
}
END_TEST

START_TEST (is_ending)
{
    int             re;
    chula_buffer_t  b   = CHULA_BUF_INIT;

    re = chula_buffer_is_ending (&b, ' ');
    ck_assert (re == 0);

    re = chula_buffer_is_ending (&b, 'a');
    ck_assert (re == 0);

    chula_buffer_add_str (&b, "12345");

    re = chula_buffer_is_ending (&b, '4');
    ck_assert (re == 0);

    re = chula_buffer_is_ending (&b, '5');
    ck_assert (re == 1);
}
END_TEST

START_TEST (move_to_begin)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    chula_buffer_add_str (&b, "0123456789");
    ret = chula_buffer_move_to_begin (&b, 11);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);

    chula_buffer_add_str (&b, "0123456789");
    ret = chula_buffer_move_to_begin (&b, 0);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "0123456789");

    ret = chula_buffer_move_to_begin (&b, 3);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "3456789");
}
END_TEST

START_TEST (ensure_size)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ck_assert (b.buf == NULL);
    ck_assert (b.size == 0);

    ret = chula_buffer_ensure_size (&b, 100);
    ck_assert (ret == ret_ok);
    ck_assert (b.buf != NULL);
    ck_assert (b.size == 100);

    ret = chula_buffer_ensure_size (&b, 99);
    ck_assert (ret == ret_ok);
    ck_assert (b.buf != NULL);
    ck_assert (b.size == 100);

    ret = chula_buffer_ensure_size (&b, 200);
    ck_assert (ret == ret_ok);
    ck_assert (b.buf != NULL);
    ck_assert (b.size == 200);
}
END_TEST

START_TEST (drop_ending)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    chula_buffer_add_str (&b, "0123456789");

    ret = chula_buffer_drop_ending (&b, 0);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 10);
    ck_assert_str_eq (b.buf, "0123456789");

    ret = chula_buffer_drop_ending (&b, 2);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 8);
    ck_assert_str_eq (b.buf, "01234567");

    ret = chula_buffer_drop_ending (&b, 999);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);
}
END_TEST

START_TEST (swap_chars)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    chula_buffer_add_str (&b, "0123401234");

    ret = chula_buffer_swap_chars (&b, '2', '*');
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 10);
    ck_assert_str_eq (b.buf, "01*3401*34");

    ret = chula_buffer_swap_chars (&b, 'Z', '$');
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 10);
    ck_assert_str_eq (b.buf, "01*3401*34");

    chula_buffer_clean (&b);
    ret = chula_buffer_swap_chars (&b, 'Z', '$');
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);
}
END_TEST

START_TEST (remove_dups)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_remove_dups (&b, 'Z');
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);

    chula_buffer_add_str (&b, "abcccde");
    ret = chula_buffer_remove_dups (&b, 'c');
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "abcde");

    chula_buffer_clean (&b);
    chula_buffer_add_str (&b, "aaaaaaa");
    ret = chula_buffer_remove_dups (&b, 'a');
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 1);
    ck_assert_str_eq (b.buf, "a");
}
END_TEST

START_TEST (remove_string)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_remove_string (&b, NULL, 0);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);

    chula_buffer_add_str (&b, "hola hola caracola");
    ret = chula_buffer_remove_string (&b, "hola ", 5);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "caracola");

    ret = chula_buffer_remove_string (&b, "bye", 3);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "caracola");

    ret = chula_buffer_remove_string (&b, "raco", 0);
    ck_assert (ret == ret_ok);
    ck_assert_str_eq (b.buf, "caracola");
}
END_TEST

START_TEST (remove_chunk)
{
    ret_t           ret;
    chula_buffer_t  b    = CHULA_BUF_INIT;

    ret = chula_buffer_remove_chunk (&b, 0, 0);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);
    ck_assert (b.buf == NULL);

    chula_buffer_add_str (&b, "0123456789");
    ret = chula_buffer_remove_chunk (&b, 11, 2);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 10);
    ck_assert_str_eq (b.buf, "0123456789");

    ret = chula_buffer_remove_chunk (&b, 5, 4);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 6);
    ck_assert_str_eq (b.buf, "012349");

    ret = chula_buffer_remove_chunk (&b, 0, 99);
    ck_assert (ret == ret_ok);
    ck_assert (b.len == 0);
}
END_TEST

START_TEST (cmp_buf)
{
    int             re;
    chula_buffer_t  a   = CHULA_BUF_INIT;
    chula_buffer_t  b   = CHULA_BUF_INIT;

    re = chula_buffer_cmp_buf (&a, &b);
    ck_assert (re == 0);

    chula_buffer_add_str (&a, "a");
    re = chula_buffer_cmp_buf (&a, &b);
    ck_assert (re > 0);
    re = chula_buffer_cmp_buf (&b, &a);
    ck_assert (re < 0);

    chula_buffer_add_str (&a, "bcd");
    chula_buffer_add_str (&b, "abcd");
    re = chula_buffer_cmp_buf (&a, &b);
    ck_assert (re == 0);
    re = chula_buffer_cmp_buf (&b, &a);
    ck_assert (re == 0);
}
END_TEST

START_TEST (cmp)
{
    int             re;
    chula_buffer_t  b   = CHULA_BUF_INIT;

    re = chula_buffer_cmp (&b, NULL, 0);
    ck_assert (re == 0);

    re = chula_buffer_cmp (&b, "a", 1);
    ck_assert (re < 0);

    chula_buffer_add_str (&b, "hola");
    re = chula_buffer_cmp (&b, "a", 1);
    ck_assert (re > 0);

    re = chula_buffer_cmp (&b, "ho", 2);
    ck_assert (re > 0);

    re = chula_buffer_cmp (&b, "hola", 2);
    ck_assert (re > 0);

    re = chula_buffer_cmp (&b, "hola", 4);
    ck_assert (re == 0);
}
END_TEST

int
buffer_tests (void)
{
    Suite *s1 = suite_create("Buffer");
    check_add (s1, init_heap);
    check_add (s1, init_ptr);
    check_add (s1, dup);
    check_add (s1, add);
    check_add (s1, slice);
    check_add (s1, add_fsize);
    check_add (s1, add_long10);
    check_add (s1, add_llong10);
    check_add (s1, add_long16);
    check_add (s1, add_llong16);
    check_add (s1, add_va);
    check_add (s1, add_char);
    check_add (s1, add_char_n);
    check_add (s1, prepend);
    check_add (s1, is_ending);
    check_add (s1, move_to_begin);
    check_add (s1, ensure_size);
    check_add (s1, drop_ending);
    check_add (s1, swap_chars);
    check_add (s1, remove_dups);
    check_add (s1, remove_string);
    check_add (s1, remove_chunk);
    check_add (s1, cmp_buf);
    check_add (s1, cmp);
    run_test (s1);
}

int
main (void)
{
    int ret;

    ret = buffer_tests();

    return ret;
}
