/** @file
 * @brief tests requiring a database backend supporting transactions
 */
/* Copyright (C) 2006,2009,2018,2023 Olly Betts
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <config.h>

#include "api_transdb.h"

#include <xapian.h>

#include "apitest.h"
#include "testutils.h"

using namespace std;

/// Test incorrect uses of the transaction API lead to errors.
DEFINE_TESTCASE(badtransaction1, transactions) {
    Xapian::WritableDatabase db(get_writable_database("apitest_simpledata"));

    TEST_EXCEPTION(Xapian::InvalidOperationError, db.commit_transaction());
    TEST_EXCEPTION(Xapian::InvalidOperationError, db.cancel_transaction());

    db.begin_transaction();
    TEST_EXCEPTION(Xapian::InvalidOperationError, db.begin_transaction());
    db.commit_transaction();

    TEST_EXCEPTION(Xapian::InvalidOperationError, db.commit_transaction());
    TEST_EXCEPTION(Xapian::InvalidOperationError, db.cancel_transaction());

    db.begin_transaction();
    TEST_EXCEPTION(Xapian::InvalidOperationError, db.begin_transaction());
    db.cancel_transaction();

    TEST_EXCEPTION(Xapian::InvalidOperationError, db.commit_transaction());
    TEST_EXCEPTION(Xapian::InvalidOperationError, db.cancel_transaction());

    db.begin_transaction();
    db.commit_transaction();

    db.begin_transaction();
    db.cancel_transaction();
}

/// Test committing a simple transaction.
DEFINE_TESTCASE(committransaction1, transactions) {
    Xapian::WritableDatabase db(get_writable_database("apitest_simpledata"));

    Xapian::doccount docs = db.get_doccount();
    db.begin_transaction();
    Xapian::Document doc;
    doc.set_data("testing");
    doc.add_term("befuddlement");
    db.add_document(doc);
    TEST_EXCEPTION(Xapian::InvalidOperationError, db.begin_transaction());
    TEST_EQUAL(db.get_doccount(), docs + 1);
    TEST_EQUAL(db.get_termfreq("befuddlement"), 1);
    db.commit_transaction();
    TEST_EQUAL(db.get_doccount(), docs + 1);
    TEST_EQUAL(db.get_termfreq("befuddlement"), 1);
}

/// Test cancelling a simple transaction.
DEFINE_TESTCASE(canceltransaction1, transactions) {
    Xapian::WritableDatabase db(get_writable_database("apitest_simpledata"));

    Xapian::doccount docs = db.get_doccount();
    db.begin_transaction();
    Xapian::Document doc;
    doc.set_data("testing");
    doc.add_term("befuddlement");
    doc.add_value(42, "answer");
    db.add_document(doc);
    TEST_EXCEPTION(Xapian::InvalidOperationError, db.begin_transaction());
    TEST_EQUAL(db.get_doccount(), docs + 1);
    TEST_EQUAL(db.get_termfreq("befuddlement"), 1);
    TEST_EQUAL(db.get_value_freq(42), 1);
    TEST_EQUAL(db.get_value_lower_bound(42), "answer");
    TEST_EQUAL(db.get_value_upper_bound(42), "answer");
    db.cancel_transaction();
    TEST_EQUAL(db.get_doccount(), docs);
    TEST_EQUAL(db.get_termfreq("befuddlement"), 0);
    TEST_EQUAL(db.get_value_freq(42), 0);
    TEST_EQUAL(db.get_value_lower_bound(42), "");
    TEST_EQUAL(db.get_value_upper_bound(42), "");
}

/// Test that begin_transaction() commits any changes pending before the
//  transaction.
DEFINE_TESTCASE(canceltransaction2, transactions) {
    Xapian::WritableDatabase db(get_writable_database("apitest_simpledata"));

    Xapian::doccount docs = db.get_doccount();
    Xapian::Document doc0;
    doc0.set_data("pending");
    doc0.add_term("pending_update");
    Xapian::docid docid = db.add_document(doc0);

    db.begin_transaction();
    TEST_EQUAL(db.get_doccount(), docs + 1);
    Xapian::Document doc;
    doc.set_data("testing");
    doc.add_term("befuddlement");
    db.add_document(doc);
    TEST_EQUAL(db.get_doccount(), docs + 2);
    db.cancel_transaction();

    TEST_EQUAL(db.get_doccount(), docs + 1);
    TEST(db.term_exists("pending_update"));
    Xapian::Document doc_out = db.get_document(docid);
    TEST_EQUAL(doc_out.get_data(), "pending");
}

/// Regression test for glass bug fixed in 1.4.6 and 1.5.0.
DEFINE_TESTCASE(canceltransaction3, transactions && path) {
    {
	Xapian::WritableDatabase db = get_named_writable_database("canceltransaction3");
	db.begin_transaction();
	Xapian::Document doc;
	doc.add_term("baz");
	db.add_document(doc);
	db.cancel_transaction();
	db.add_document(doc);
	db.commit();
    }

    size_t check_errors =
	Xapian::Database::check(get_named_writable_database_path("canceltransaction3"),
				Xapian::DBCHECK_SHOW_STATS, &tout);
    TEST_EQUAL(check_errors, 0);
}
