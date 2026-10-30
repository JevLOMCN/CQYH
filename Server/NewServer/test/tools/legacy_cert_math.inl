// Legacy generator retained as an independent compatibility reference.
static const INT32 s_nAdjacentX[DT_DIRECTION_8_COUNT] =
{
	1,		//	DT_EAST,
	1,		//	DT_SOUTH_EAST,
	0,		//  DT_SOUTH,
	-1,		//  DT_SOUTH_WEST,
	-1,		//  DT_WEST,
	-1,		//  DT_NORTH_WEST,
	0,		//  DT_NORTH,
	1		//  DT_NORTH_EAST,
};

static const INT32 s_nAdjacentY[DT_DIRECTION_8_COUNT] =
{
	0,		//	DT_EAST,
	1,		//	DT_SOUTH_EAST,
	1,		//  DT_SOUTH,
	1,		//  DT_SOUTH_WEST,
	0,		//  DT_WEST,
	-1,		//  DT_NORTH_WEST,
	-1,		//  DT_NORTH,
	-1		//  DT_NORTH_EAST,
};

static const INT32 s_nAdjList[DT_DIRECTION_8_COUNT][6] =
{
	{ 3, DT_NORTH_EAST, DT_EAST, DT_SOUTH_EAST },							// DT_EAST,
	{ 5, DT_NORTH_EAST, DT_EAST, DT_SOUTH_EAST, DT_SOUTH, DT_SOUTH_WEST },	// DT_SOUTH_EAST,
	{ 3, DT_SOUTH_EAST, DT_SOUTH, DT_SOUTH_WEST },							// DT_SOUTH,
	{ 5, DT_SOUTH_EAST, DT_SOUTH, DT_SOUTH_WEST, DT_WEST, DT_NORTH_WEST },	// DT_SOUTH_WEST,
	{ 3, DT_SOUTH_WEST, DT_WEST, DT_NORTH_WEST },							// DT_WEST,
	{ 5, DT_SOUTH_WEST, DT_WEST, DT_NORTH_WEST, DT_NORTH, DT_NORTH_EAST },	// DT_NORTH_WEST,
	{ 3, DT_NORTH_WEST, DT_NORTH, DT_NORTH_EAST },							// DT_NORTH,
	{ 5, DT_NORTH_WEST, DT_NORTH, DT_NORTH_EAST, DT_EAST, DT_SOUTH_EAST },	// DT_NORTH_EAST,
};

void legacy_cert_math(MATH_TABLE& table)
{
    memset(&table, 0, sizeof(table));

    for (int nDirection = 0; nDirection < MAX_DIRECTION_COUNT; nDirection++)
    {
        table.nSinTable[nDirection] = (INT32)floor(sin((double)nDirection * 2 * PI / MAX_DIRECTION_COUNT) * SIN_COS_NUMBER + 0.5f);
        table.nCosTable[nDirection] = (INT32)floor(cos((double)nDirection * 2 * PI / MAX_DIRECTION_COUNT) * SIN_COS_NUMBER + 0.5f);
    }

    for (int nDirection = 0; nDirection < MAX_DIRECTION_COUNT/4; nDirection++)
    {
        table.nSinSquareTable[nDirection] = table.nSinTable[nDirection] * table.nSinTable[nDirection];
    }

    for(int i = 0; i < MAX_SQUARE; i++)
    {
        table.wSqrtTable[i] = (UINT16)floor(sqrt((double)i) + 0.5f);
    }

	memcpy(table.nAdjacentX, s_nAdjacentX, sizeof(table.nAdjacentX));
	memcpy(table.nAdjacentY, s_nAdjacentY, sizeof(table.nAdjacentY));
	memcpy(table.nAdjList, s_nAdjList, sizeof(table.nAdjList));
}
