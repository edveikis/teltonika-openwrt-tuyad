local ConfigService = require("api/ConfigService")

local Tuyad = ConfigService:new({
	create = false,
	delete = false,
	general_section = "main",
})

local Daemon = Tuyad:section(
	"tuyad",
	"tuyad"
)

Daemon:make_primary()

--
-- Options
--
local opt_enabled = Daemon:option("enabled")
	function opt_enabled:validate(value)
		return self.dt:is_bool(value)
	end

local opt_dev_id = Daemon:option("dev_id")
	opt_dev_id.cfg_require = true
	opt_dev_id.minlength = 1
	opt_dev_id.maxlength = 23

local opt_dev_secret = Daemon:option("dev_secret")
	opt_dev_secret.cfg_require = true
	opt_dev_secret.minlength = 1
	opt_dev_secret.maxlength = 17

local opt_product_id = Daemon:option("product_id")
	opt_product_id.cfg_require = true
	opt_product_id.maxlength = 17

local opt_interval = Daemon:option("interval")
	function opt_interval:validate(value)
		local n = tonumber(value)
		if not n or n < 1 then
			return false
		end
		return true
	end

local opt_send_now = Daemon:option("send_now")
	function opt_send_now:validate(value)
		return self.dt:is_bool(value)
	end

return Tuyad