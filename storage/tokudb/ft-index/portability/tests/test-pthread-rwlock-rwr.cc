/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
// vim: ft=cpp:expandtab:ts=8:sw=4:softtabstop=4:
#ident "$Id$"
/*
COPYING CONDITIONS NOTICE:

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation, and provided that the
  following conditions are met:

      * Redistributions of source code must retain this COPYING
        CONDITIONS NOTICE, the COPYRIGHT NOTICE (below), the
        DISCLAIMER (below), the UNIVERSITY PATENT NOTICE (below), the
        PATENT MARKING NOTICE (below), and the PATENT RIGHTS
        GRANT (below).

      * Redistributions in binary form must reproduce this COPYING
        CONDITIONS NOTICE, the COPYRIGHT NOTICE (below), the
        DISCLAIMER (below), the UNIVERSITY PATENT NOTICE (below), the
        PATENT MARKING NOTICE (below), and the PATENT RIGHTS
        GRANT (below) in the documentation and/or other materials
        provided with the distribution.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.

COPYRIGHT NOTICE:

  TokuDB, Tokutek Fractal Tree Indexing Library.
  Copyright (C) 2007-2013 Tokutek, Inc.

DISCLAIMER:

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

UNIVERSITY PATENT NOTICE:

  The technology is licensed by the Massachusetts Institute of
  Technology, Rutgers State University of New Jersey, and the Research
  Foundation of State University of New York at Stony Brook under
  United States of America Serial No. 11/760379 and to the patents
  and/or patent applications resulting from it.

PATENT MARKING NOTICE:

  This software is covered by US Patent No. 8,185,551.
  This software is covered by US Patent No. 8,489,638.

PATENT RIGHTS GRANT:

  "THIS IMPLEMENTATION" means the copyrightable works distributed by
  Tokutek as part of the Fractal Tree project.

  "PATENT CLAIMS" means the claims of patents that are owned or
  licensable by Tokutek, both currently or in the future; and that in
  the absence of this license would be infringed by THIS
  IMPLEMENTATION or by using or running THIS IMPLEMENTATION.

  "PATENT CHALLENGE" shall mean a challenge to the validity,
  patentability, enforceability and/or non-infringement of any of the
  PATENT CLAIMS or otherwise opposing any of the PATENT CLAIMS.

  Tokutek hereby grants to you, for the term and geographical scope of
  the PATENT CLAIMS, a non-exclusive, no-charge, royalty-free,
  irrevocable (except as stated in this section) patent license to
  make, have made, use, offer to sell, sell, import, transfer, and
  otherwise run, modify, and propagate the contents of THIS
  IMPLEMENTATION, where such license applies only to the PATENT
  CLAIMS.  This grant does not include claims that would be infringed
  only as a consequence of further modifications of THIS
  IMPLEMENTATION.  If you or your agent or licensee institute or order
  or agree to the institution of patent litigation against any entity
  (including a cross-claim or counterclaim in a lawsuit) alleging that
  THIS IMPLEMENTATION constitutes direct or contributory patent
  infringement, or inducement of patent infringement, then any rights
  granted to you under this License shall terminate as of the date
  such litigation is filed.  If you or your agent or exclusive
  licensee institute or order or agree to the institution of a PATENT
  CHALLENGE, then Tokutek may terminate any rights granted to you
  under this License.
*/

#ident "Copyright (c) 2007-2013 Tokutek Inc.  All rights reserved."
#ident "The technology is licensed by the Massachusetts Institute of Technology, Rutgers State University of New Jersey, and the Research Foundation of State University of New York at Stony Brook under United States of America Serial No. 11/760379 and to the patents and/or patent applications resulting from it."
#include <test.h>
#include <toku_assert.h>
#include <toku_pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// write a test see if things happen in the right order.

volatile int state = 0;
int verbose = 0;

static void *f(void *arg) {
    toku_pthread_rwlock_t *mylock = (toku_pthread_rwlock_t *) arg;
    sleep(2);
    assert(state==42); state = 16; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);
    toku_pthread_rwlock_wrlock(mylock);
    assert(state==49); state = 17; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);
    toku_pthread_rwlock_wrunlock(mylock);
    sleep(10);
    assert(state==52); state = 20; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);
    return arg;
}

int test_main(int argc , char *const argv[] ) {
    assert(argc==1 || argc==2);
    if (argc==2) {
	assert(strcmp(argv[1],"-v")==0);
	verbose = 1;
    }
    int r;
    toku_pthread_rwlock_t rwlock;
    toku_pthread_t tid;
    void *retptr;

    toku_pthread_rwlock_init(&rwlock, NULL);
    state = 37; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);
    toku_pthread_rwlock_rdlock(&rwlock);

    r = toku_pthread_create(&tid, NULL, f, &rwlock); assert(r == 0);

    assert(state==37); state = 42; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);
    sleep(4);
    assert(state==16); state = 44; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);
    toku_pthread_rwlock_rdlock(&rwlock);
    assert(state==44); state = 46; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);
    toku_pthread_rwlock_rdunlock(&rwlock);
    sleep(4);
    assert(state==46); state=49; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__); // still have a read lock
    toku_pthread_rwlock_rdunlock(&rwlock);
    sleep(6);
    assert(state==17); state=52; if (verbose) printf("%s:%d\n", __FUNCTION__, __LINE__);

    r = toku_pthread_join(tid, &retptr); assert(r == 0);

    toku_pthread_rwlock_destroy(&rwlock);
    
    return 0;
}
