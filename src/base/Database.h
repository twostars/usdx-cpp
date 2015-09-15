#ifndef _DATABASE_H
#define _DATABASE_H
#pragma once

#include "UsdxDatabase.h"

class Database : public Singleton<Database>
{
public:
	Database();
	bool Init(const path& scorePath);
	~Database();

private:
	UsdxDatabase _database;
};

#define sDatabase (Database::getSingleton())

#endif
