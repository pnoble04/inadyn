/* Small cmd line program useful for maintaining an IP address in a Dynamic DNS system.
 *
 * Copyright (C) 2003-2004  Narcis Ilisei <inarcis2002@hotpop.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#define MODULE_TAG "INADYN: "
#include <stdlib.h>

#include "debug_if.h"
#include "dyndns.h"
#include "errorcode.h"

/* MAIN - Dyn DNS update entry point.*/
int inadyn_main(int argc, char* argv[])
{
	int restart = 0;
	BOOL os_handler_installed = FALSE;
	RC_TYPE rc = RC_OK;
	DYN_DNS_CLIENT *p_dyndns = NULL;

	do
	{
		/* create DYN_DNS_CLIENT object	*/
		rc = dyn_dns_construct(&p_dyndns);
		if (rc != RC_OK)
		{
			break;
		}

		/* install signal handler */
		if (!os_handler_installed)
		{
			rc = os_install_signal_handler(p_dyndns);
			if (rc != RC_OK)
			{
				logit(LOG_WARNING, MODULE_TAG  "Failed installing OS signal handler: Error '%s' (0x%x)\n", errorcode_get_name(rc), rc);
				break;
			}
			os_handler_installed = TRUE;
		}

		rc = dyn_dns_main(p_dyndns, argc, argv);
		if (rc == RC_RESTART)
		{
			restart = 1;

			/* do some cleanup if restart requested */
			rc = dyn_dns_destruct(p_dyndns);
			if (rc != RC_OK)
			{
				logit(LOG_WARNING, MODULE_TAG "Failed cleanup on restart: Error '%s' (0x%x) in dyn_dns_destruct().\n", errorcode_get_name(rc), rc);
			}
		}
		else
		{
			/* Error, or OK.  In either case exit outer loop. */
			restart = 0;
		}
	}
	while (restart);

	/* destroy DYN_DNS_CLIENT object*/
	rc = dyn_dns_destruct(p_dyndns);
	if (rc != RC_OK)
	{
		logit(LOG_WARNING, MODULE_TAG "Failed cleanup before exit: '%s' (0x%x) in dyn_dns_destruct().\n", errorcode_get_name(rc), rc);
	}

	os_close_dbg_output();

	return (int)rc;
}

/**
 * Local Variables:
 *  version-control: t
 *  indent-tabs-mode: t
 *  c-file-style: "ellemtel"
 *  c-basic-offset: 8
 * End:
 */
