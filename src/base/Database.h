#ifndef _DATABASE_H
#define _DATABASE_H
#pragma once

class Database : public Singleton<Database>
{
public:
	Database();
	bool Init(path& scorePath);
	void Close();
	~Database();

private:
	struct sqlite3 * _database;
};

#define sDatabase (Database::getSingleton())

#endif
