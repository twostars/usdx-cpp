#ifndef _USDXDATABASE_H
#define _USDXDATABASE_H
#pragma once

#include "Sqlite3Database.h"

class UsdxDatabase : public Sqlite3Database
{
public:
	void Initialize();
	void ConvertFrom101To110();
};

#endif
