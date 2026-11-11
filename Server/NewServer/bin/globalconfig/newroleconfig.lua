NewRoleConfig = {
    nSceneTempID = 142,
    nX = 26002,
    nY = 56875,
    nZ = 4748,
    wFaceDirection = 5120,

    MaleNewEquip = {
	-- Warrior --
        [0] = {
            nMainHand = 99900001,
      --      nHead = 0,
      --      nShoulder = 0,
            nBody = 99980113,
      --      nArm = 0,
      --      nWaist = 0,
      --      nFoot = 0,
	  --      nBack = 0
        },
	-- Mage --
        [1] = {
            nMainHand = 99900001,
      --       nHead = 0,
      --       nShoulder = 0,
            nBody = 99980113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = 0,
      --       nBack = 0
        },
	-- Assassin --
        [2] = {
             nMainHand = 99900001,
      --       nHead = 0,
      --       nShoulder = 0,
            nBody = 99980113,
      --       nArm =0 ,
      --       nWaist =0 ,
      --       nFoot =0 ,
      --       nBack = 0
        },
	-- Archer --
        [3] = {
            nMainHand = 99900001,
	  --       nHead = 0,
      --       nShoulder = 0,
            nBody = 99980113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = 0,
      --       nBack = 0
        },
	-- Taoist --
        [4] = {
            nMainHand = 99900001,
      --       nHead = 0,
      --       nShoulder = 0,
            nBody = 89980113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = ,
      --       nBack = 0
        },
	-- Spear (1.05 Dragon Lancer) --
        [5] = {
            nMainHand = 99900100,
            nBody = 99980113,
        },
	-- Sorcerer (1.05 Demon) --
        [6] = {
            nMainHand = 99900199,
            nBody = 99980113,
        },
	},
    FemaleNewEquip = {
	-- Warrior --
        [0] = {
            nMainHand = 99900001,
      --       nHead = 0,
      --       nShoulder = 0,
            nBody = 99990113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = 0,
      --       nBack = 0
        },
	-- Mage --
        [1] = {
            nMainHand = 99900001,
      --       nHead = 0,
      --       nShoulder = 0,
            nBody = 99990113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = 0,
      --       nBack = 0
        },
	-- Assassin --
       [2] = {
             nMainHand = 99900001,
      --       nHead = 0,
      --        nShoulder = 0,
            nBody = 99990113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = 0,
      --       nBack = 0
        },
	-- Archer --
       [3] = {
            nMainHand = 99900001,
      --       nHead = 0,
      --       nShoulder = 0,
            nBody = 99990113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = 0,
      --       nBack = 0
        },
	-- Taoist --
        [4] = {
            nMainHand = 99900001,
      --       nHead = 0,
      --       nShoulder = 0,
            nBody = 99990113,
      --       nArm = 0,
      --       nWaist = 0,
      --       nFoot = 0,
      --       nBack = 0
        },
	-- Spear (1.05 Dragon Lancer) --
        [5] = {
            nMainHand = 99900100,
            nBody = 99990113,
        },
	-- Sorcerer (1.05 Demon) --
        [6] = {
            nMainHand = 99900199,
            nBody = 99990113,
        },
	}
};

NewRoleConfig.nX, NewRoleConfig.nY, NewRoleConfig.nZ =
	ConvertCoordO2N(NewRoleConfig.nX, NewRoleConfig.nY, NewRoleConfig.nZ);
