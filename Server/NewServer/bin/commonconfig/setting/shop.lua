shop_define = {
	owner='Tan Yong',
	Index = {
		type = 'number',
		default = 0,
		min = 0,
	},
	ID	= {
		type = 'number',
		default = 0,
		min = 0,
	},
	ShopName = {
		type = 'string',
		default = "Default",
		maxlen = 256,
	},	
}

for i=1,256 do
	shop_define['ItemID'..i] = {
		type = 'item',
		default = 0,
	};
	shop_define['ItemCount'..i] = {
		type = 'number',
		default = 0,
		min = 0,
	};
	shop_define['CostItemID'..i] = {
		type = 'number',
		default = 0,
		min = 0,
	};
	shop_define['CostValue'..i] = {
		type = 'number',
		default = 0,
		min = 0,
	};
end