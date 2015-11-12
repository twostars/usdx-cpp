#ifndef _SQLITEDATABASE_H
#define _SQLITEDATABASE_H
#pragma once

class Sqlite3Exception : public std::exception
{
public:
	explicit Sqlite3Exception(const std::string& what_arg)
		: std::exception(what_arg.c_str())
	{
	}
};

class Sqlite3NoRowsException : public Sqlite3Exception
{
public:
	explicit Sqlite3NoRowsException(const std::string& what_arg)
		: Sqlite3Exception(what_arg)
	{
	}
};

class Sqlite3Database
{
public:
	struct Sqlite3ExecContext
	{
		int(*callback)(void * param, int argc, char ** argv, char ** azColNames) = nullptr;
		void * param = nullptr;
		char ** errmsg = nullptr;
	};

	Sqlite3Database();
	int Open(const path& filePath);
	int ExecCtx(Sqlite3ExecContext * context, const char * sql);
	int FormattedExecCtx(Sqlite3ExecContext * context, const char * sql, ...);
	int Exec(const char * sql);
	int FormattedExec(const char * sql, ...);
	char * ErrMsg();
	Sint32 GetUserVersion();
	void SetUserVersion(Sint32 version);
	bool TableExists(const char * table);
	bool ColumnExists(const char * table, const char * column);

	template <typename T>
	T GetTableValue(const char * sql, ...);
	std::string GetTableString(const char * sql, ...);

	int Close();
	~Sqlite3Database();

protected:
	struct sqlite3 * _database;
};

#endif
