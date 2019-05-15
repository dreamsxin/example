// action_description.cpp : implement file for action description interface
//

#include "stdafx.h"
#include "action_description.h"


ACTION_DESCRIPTION actDesc[] = 
{
    { ONE_MONSTER_GO ,            "One monster go over river"              },
    { TWO_MONSTER_GO ,            "Two monster go over river"              },
    { ONE_MONK_GO ,               "One monk go over river"                 },
    { TWO_MONK_GO ,               "Two monk go over river"                 },
    { ONE_MONSTER_ONE_MONK_GO ,   "One monster and one monk go over river" },
    { ONE_MONSTER_BACK ,          "One monster go back"                    },
    { TWO_MONSTER_BACK ,          "Two monster go back"                    },
    { ONE_MONK_BACK ,             "One monk go back"                       },
    { TWO_MONK_BACK ,             "Two monk go back"                       },
    { ONE_MONSTER_ONE_MONK_BACK , "One monster and one monk go back"       }
};


const char *GetActionDescription(ACTION_NAME act)
{
    int actCount = sizeof(actDesc) / sizeof(actDesc[0]);
    for(int i = 0; i < actCount; ++i)
    {
        if(actDesc[i].act == act)
        {
            return actDesc[i].description;
        }
    }

    return "Unknown action";
}
