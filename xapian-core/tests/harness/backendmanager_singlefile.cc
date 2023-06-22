/** @file
 * @brief BackendManager subclass for singlefile databases.
 */
/* Copyright (C) 2007,2008,2009,2011,2012,2013,2015,2018 Olly Betts
 * Copyright (C) 2008 Lemur Consulting Ltd
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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

#include "backendmanager_singlefile.h"

#include "filetests.h"
#include "index_utils.h"
#include "unixcmds.h"

#include <cstdio> // For rename().

using namespace std;

BackendManagerSingleFile::BackendManagerSingleFile(const string& datadir_,
						   BackendManager* sub_manager_)
    : BackendManager(datadir_),
      sub_manager(sub_manager_),
      cachedir(".singlefile" + sub_manager_->get_dbtype())
{
    // Ensure the directory we store cached test databases in exists.
    (void)create_dir_if_needed(cachedir);
}

std::string
BackendManagerSingleFile::get_dbtype() const
{
    return "singlefile_" + sub_manager->get_dbtype();
}

string
BackendManagerSingleFile::do_get_database_path(const vector<string> & files)
{
    string db_path = cachedir + "/db";
    for (const string& file : files) {
	db_path += "__";
	db_path += file;
    }

    if (!file_exists(db_path)) {
	// No cached DB exists.  Create at a temporary path and rename
	// so we don't leave a partial DB in place upon failure.
	string tmp_path = db_path + ".tmp";
	sub_manager->get_database(files).compact(tmp_path,
						 Xapian::DBCOMPACT_SINGLE_FILE);
	if (rename(tmp_path.c_str(), db_path.c_str()) < 0) {
	    throw Xapian::Database("rename failed", errno);
	}
    }

    return db_path;
}

Xapian::WritableDatabase
BackendManagerSingleFile::get_writable_database(const string &, const string &)
{
    throw Xapian::UnimplementedError("Single-file databases don't support writing");
}

string
BackendManagerSingleFile::get_compaction_output_path(const string& name)
{
    return cachedir + "/" + name;
}
