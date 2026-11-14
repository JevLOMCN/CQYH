LoadPackage("error_def.pkg")

Include("script/include/command_table.lua");
Include("script/ms_gm/user_permission.lua");


function CheckIP(pcszIP)
	if ip_table[pcszIP] == nil then
		return 0;
	else
		return 1;
	end
end

function CheckPassword(pcszName, pcszPassword)
	print(pcszName);
	print(pcszPassword);

	if permission_table[pcszName] ~= nil then
		print (permission_table[pcszName][1]);
		if permission_table[pcszName][1] == pcszPassword then
			return 1;
		end
	end
	return 0;
end




--help = {};
--
function echo(role_id, text)
	say(role_id, role_id, text);
end
--

command_cache = {};

function OnMSGMCommand(command, self_id, self_name)
	local fstr;
	local target_id = self_id;

	command = command..' ';
	local func = string.lower(string.match(command, "%s*([_(),%w]+)%s*"));

	if gm[func] and (permission_table[self_name] or TEST_ENVIRONMENT)then
		local index = 1;
		fstr = gm[func][3]..'(';
		for w in string.gmatch(command, "[%s,]*(.-)[%s,]+") do
			if index == 1 then

			elseif index == 2 then
				if tostring(tonumber(w)) == w or _G[w] then
					fstr = fstr..w;
				else
					fstr = fstr.."'"..w.."'";
				end
			else
				if tostring(tonumber(w)) == w or _G[w] then
					fstr = fstr..','..w;
				else
					fstr = fstr..','.."'"..w.."'";
				end
			end
			index = index + 1;
		end
		fstr = fstr..');';
	else
		if nil == TEST_ENVIRONMENT then
			return false;
		end
		fstr = command;
	end

	if gm[func] then
		if gm[func][1] == MS_GM_COMMAND then
			if command_cache[fstr] then
				local f = command_cache[fstr];
				echo(self_id, command);
				f();
				return true;
			else
				local f, msg = loadstring(fstr);
				if f then
					echo(self_id, command);
					command_cache[fstr] = f;
					f();
					return true;
				elseif msg then
					echo(self_id, 'Invalid GM command '..msg);
				end
			end
		elseif gm[func][1] == GS_GM_COMMAND then
			-- send to gs to execute
			local ret = call_gs_gm_func(self_id, fstr);

			if 1 == ret then
				return true;
			else
				return false;
			end
		end
	elseif func == 'run' then
		fstr = string.sub(command, 4, 9999);
		print(fstr);
		if command_cache[fstr] then
			local f = command_cache[fstr];
			echo(self_id, command);
			f();
			return true;
		else
			local f, msg = loadstring(fstr);
			if f then
				echo(self_id, command);
				command_cache[fstr] = f;
				f();
				return true;
			elseif msg then
				echo(self_id, 'Invalid GM command '..msg);
			end
		end
		return true;
	else
		-- send to gs to execute
		local ret = call_gs_gm_func(self_id, command);

		if 1 == ret then
			return true;
		else
			return false;
		end
	end

	return false;
end


