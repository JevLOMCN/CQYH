#ifndef __MESSAGE_HANDLER_ITEM_H__
#define __MESSAGE_HANDLER_ITEM_H__

public:
	void Send_MoveItem( INT DstConSlot , INT DstItemSlot , INT SrcConSlot , INT SrcItemSlot );
	void Send_UseItem( INT ConSlot, INT ItemSlot, INT TemplateID );
	void Send_DeleteItem( INT ConSlot , INT ItemSlot , INT ItemCount , INT TemplateId );
	void Send_RepairItem( INT ConSlot , INT ItemSlot );
	void Send_SpecRepairItem( INT ConSlot , INT ItemSlot );
	void Send_RepairAll();
	void Send_SplitItem( UINT16 srcConSlot , UINT16 srcItemSlot, UINT16 dstConSlot , UINT16 dstItemSlot, INT32 count);
	void Send_SortBag(UINT16 containerID);

	//Identify
	void Send_IdentifyItem( INT ConSlot, INT ItemSlot );
	//Signature
	void Send_SignatureItem( INT ConSlot, INT ItemSlot );

	//Inscription
	void Send_TryGetWeaponRune( INT weaponConSlot, INT weaponItemSlot, INT itemConsumeID );
	void Send_TryReplaceWeaponRune( INT weaponConSlot, INT weaponItemSlot, INT itemConsumeID );
	void Send_ConfirmReplaceWeaponRune( INT weaponConSlot, INT weaponItemSlot );
	//Enhancement
	//void Send_Refine( UINT16 wItemCon, UINT16 wItemSlot );
	//void Send_Unrefine( UINT16 wItemSlot, UINT8 bAll );

	//Weapon upgrade
	//void Send_EquipLevelUp( const struct FOperationItemBagPos &equipPos, 
	//						const struct FOperationItemBagPos (&materialPos)[MAX_EQUIP_LEVEL_UP_MATERIAL_COUNT], 
	//						const struct FOperationItemBagPos (&helpItemPos)[MAX_EQUIP_LEVEL_UP_LUCK_JEWELRY_COUNT] );
	void Send_EquipLevelUpShortcut_PutIn( int conID, int slotID );
	void Send_EquipLevelUpShortcut_TakeOut();
	void Send_EquipLevelUpRightNow_TakeOut();

	//Expand
	void Send_BuyContainerSlot( INT ConSlot , INT Count );

	//One-click sell
	void Send_SellAllItemInBagResource();

M_DECL_PARSE_FUN(g2c_sync_item_bag);
M_DECL_PARSE_FUN( g2c_item_change );
M_DECL_PARSE_FUN(g2c_item_delete);
M_DECL_PARSE_FUN(g2c_item_swap);
M_DECL_PARSE_FUN(g2c_sign_item);
M_DECL_PARSE_FUN(g2c_container_size_change);
M_DECL_PARSE_FUN(g2c_rune_item);
M_DECL_PARSE_FUN(g2c_rune_item_advance);
M_DECL_PARSE_FUN(g2c_rune_item_advance_confirm);
M_DECL_PARSE_FUN(g2c_luck_oil);
M_DECL_PARSE_FUN(g2c_identify_item);
M_DECL_PARSE_FUN(g2c_repair_item);
M_DECL_PARSE_FUN(g2c_lose_pk_item);
M_DECL_PARSE_FUN(g2c_destroy_death_item);
//M_DECL_PARSE_FUN(g2c_equip_level_up);
M_DECL_PARSE_FUN(g2c_weapon_level_up_put_in);
M_DECL_PARSE_FUN(g2c_weapon_level_up_take_out);
M_DECL_PARSE_FUN(g2c_weapon_level_up_result);
M_DECL_PARSE_FUN(g2c_equip_dur_change);
M_DECL_PARSE_FUN(g2c_sell_resource_item);

#define M_INIT_MSGHANDLER_ITEM	\
	MREG_PARSE_FUN(g2c_sync_item_bag);			\
	MREG_PARSE_FUN(g2c_item_delete);	\
	MREG_PARSE_FUN(g2c_item_change);	\
	MREG_PARSE_FUN(g2c_item_swap);\
	MREG_PARSE_FUN(g2c_sign_item);\
	MREG_PARSE_FUN(g2c_equip_dur_change);\
	MREG_PARSE_FUN(g2c_container_size_change);\
	MREG_PARSE_FUN(g2c_rune_item);\
	MREG_PARSE_FUN(g2c_rune_item_advance);\
	MREG_PARSE_FUN(g2c_rune_item_advance_confirm);\
	MREG_PARSE_FUN(g2c_luck_oil);\
	MREG_PARSE_FUN(g2c_identify_item);\
	MREG_PARSE_FUN(g2c_repair_item);\
	MREG_PARSE_FUN(g2c_lose_pk_item);\
	MREG_PARSE_FUN(g2c_destroy_death_item);\
	MREG_PARSE_FUN(g2c_weapon_level_up_put_in);\
	MREG_PARSE_FUN(g2c_weapon_level_up_take_out);\
	MREG_PARSE_FUN(g2c_weapon_level_up_result);\
	MREG_PARSE_FUN(g2c_sell_resource_item);
#endif