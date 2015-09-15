#include "stdafx.h"
#include "Sqlite3Database.h"
#include <sqlite/sqlite3.h>

Sqlite3Database::Sqlite3Database()
	: _database(nullptr)
{
}

int Sqlite3Database::Open(const path& filePath)
{
	if (_database != nullptr)
		Close();

	// TODO: Clean this up to accept paths as UTF-16 (i.e. via sqlite3_open_v2).
	return sqlite3_open(filePath.generic_string().c_str(), &_database);
}

int Sqlite3Database::ExecCtx(Sqlite3ExecContext * context, const char * sql)
{
	return sqlite3_exec(_database, sql, context->callback, context->param, context->errmsg);
}

int Sqlite3Database::FormattedExecCtx(Sqlite3ExecContext * context, const char * sql, ...)
{
	va_list args;
	char formattedSql[16384];

	va_start(args, sql);
	vsnprintf(formattedSql, sizeof(formattedSql), sql, args);
	va_end(args);

	return ExecCtx(context, formattedSql);
}

int Sqlite3Database::Exec(const char * sql)
{
	Sqlite3ExecContext context;
	return ExecCtx(&context, sql);
}

int Sqlite3Database::FormattedExec(const char * sql, ...)
{
	Sqlite3ExecContext context;
	va_list args;
	char formattedSql[16384];

	va_start(args, sql);
	vsnprintf(formattedSql, sizeof(formattedSql), sql, args);
	va_end(args);

	return ExecCtx(&context, formattedSql);
}

template <typename T>
T Sqlite3Database::GetTableValue(const char * sql, ...)
{
	struct Sqlite3CallbackContext
	{
		bool callbackRun = false;
		T param;
	} callbackContext;
	Sqlite3ExecContext execContext;

	va_list args;
	char formattedSql[16384];
	char * errmsg;

	va_start(args, sql);
	vsnprintf(formattedSql, sizeof(formattedSql), sql, args);
	va_end(args);

	execContext.callback = [](void * param, int argc, char ** argv, char ** azColNames)
	{
		auto context = (Sqlite3CallbackContext *)param;
		context->callbackRun = true;

		if (argc <= 0)
			return SQLITE_ERROR;

		context->param = (T)atoll(argv[0]);
		return SQLITE_OK;
	};
	execContext.param = &callbackContext;
	execContext.errmsg = &errmsg;

	int r = this->ExecCtx(&execContext, formattedSql);
	if (r != SQLITE_OK)
		throw Sqlite3Exception(errmsg);

	if (!callbackContext.callbackRun)
		throw Sqlite3NoRowsException(formattedSql);

	return callbackContext.param;
}

std::string Sqlite3Database::GetTableString(const char * sql, ...)
{
	struct Sqlite3CallbackContext
	{
		bool callbackRun = false;
		std::string param;
	} callbackContext;
	Sqlite3ExecContext execContext;

	va_list args;
	char formattedSql[16384];
	char * errmsg;

	va_start(args, sql);
	vsnprintf(formattedSql, sizeof(formattedSql), sql, args);
	va_end(args);

	execContext.callback = [](void * param, int argc, char ** argv, char ** azColNames)
	{
		auto context = (Sqlite3CallbackContext *)param;
		context->callbackRun = true;

		if (argc <= 0)
			return SQLITE_ERROR;

		context->param = argv[0];
		return SQLITE_OK;
	};
	execContext.param = &callbackContext;
	execContext.errmsg = &errmsg;

	int r = this->ExecCtx(&execContext, formattedSql);
	if (r != SQLITE_OK)
		throw Sqlite3Exception(errmsg);

	if (!callbackContext.callbackRun)
		throw Sqlite3NoRowsException(formattedSql);

	return callbackContext.param;
}

char * Sqlite3Database::ErrMsg()
{
	// NOTE: this will be freed by Sqlite.
	return (_database != nullptr ? sqlite3_errmsg(_database) : "unknown");
}

int32 Sqlite3Database::GetUserVersion()
{
	return GetTableValue<int32>("PRAGMA user_version");
}

void Sqlite3Database::SetUserVersion(int32 version)
{
	FormattedExec("PRAGMA user_version = %d", version);
}

bool Sqlite3Database::TableExists(const char * table)
{
	Sqlite3ExecContext context;
	bool exists = false;
	char * errmsg;

	context.callback = [](void * param, int argc, char ** argv, char ** azColNames)
	{
		if (argc <= 0)
			return SQLITE_ERROR;

		*(bool *)param = atoi(argv[0]) > 0;
		return SQLITE_OK;
	};
	context.param = &exists;
	context.errmsg = &errmsg;

	int r = this->FormattedExecCtx(&context, "select count(*) from sqlite_master where [type] = 'table' and lower([name]) = lower('%s')", table);
	if (r != SQLITE_OK)
		throw Sqlite3Exception(errmsg);

	return exists;
}

bool Sqlite3Database::ColumnExists(const char * table, const char * column)
{
	struct Sqlite3CallbackContext
	{
		const char * ColumnName;
		bool ColumnExists = false;
	} callbackContext;
	Sqlite3ExecContext execContext;
	char * errmsg;

	callbackContext.ColumnName = column;
	execContext.callback = [](void * param, int argc, char ** argv, char ** azColNames)
	{
		auto context = (Sqlite3CallbackContext *)param;
		if (argc <= 2)
			return SQLITE_ERROR;

		if (STRCASECMP(argv[1], context->ColumnName) == 0)
			context->ColumnExists = true;

		return SQLITE_OK;
	};
	execContext.param = &callbackContext;
	execContext.errmsg = &errmsg;

	int r = this->FormattedExecCtx(&execContext, "PRAGMA TABLE_INFO('%s')", table);
	if (r != SQLITE_OK)
		throw Sqlite3Exception(errmsg);

	return callbackContext.ColumnExists;
}

int Sqlite3Database::Close()
{
	if (_database == nullptr)
		return SQLITE_ERROR;

	int r = sqlite3_close(_database);
	if (r == SQLITE_OK)
		_database = nullptr;

	return r;
}

Sqlite3Database::~Sqlite3Database()
{
	Close();
}
