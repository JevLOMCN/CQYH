#ifndef __MESSAGE_HANDLER_DROP_H__
#define __MESSAGE_HANDLER_DROP_H__
public:
	void Send_PickDropItem( INT DropID,UINT8 iMode  );

	void TickDropAnim(FLOAT deltaTime);

protected:

	FLOAT _tickDropAnimTime;
	const FLOAT C_ClearDropAnimCounterInterval;

	struct FDropIDCounter
	{
		INT Counter;
		FLOAT LifeSpan;
	};

	typedef std::map<INT, FDropIDCounter> DropActorSrcIDCounterMap;

	DropActorSrcIDCounterMap _dropIDCounterMap;

	INT GetDropCounterFromID(INT srcID);

private:
	//Dropped item pickup related
	M_DECL_PARSE_FUN(g2c_sync_drop_list);
	M_DECL_PARSE_FUN(g2c_pickup_item);
	M_DECL_PARSE_FUN(g2c_pickup_money);
	M_DECL_PARSE_FUN(g2c_generate_drop_from_object);

#define M_INIT_MSGHANDLER_DROP \
    MREG_PARSE_FUN(g2c_sync_drop_list); \
	MREG_PARSE_FUN(g2c_pickup_item); \
	MREG_PARSE_FUN(g2c_pickup_money); \
	MREG_PARSE_FUN(g2c_generate_drop_from_object); \

    

#endif