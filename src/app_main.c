/*****************************************************************************
 *   Ledger App Boilerplate.
 *   (c) 2020 Ledger SAS.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *****************************************************************************/

#include <stdint.h>  // uint*_t
#include <string.h>  // memset, explicit_bzero

#include "os.h"
#include "ux.h"

#include "types.h"
#include "globals.h"
#include "io.h"
#include "sw.h"
#include "ui/menu.h"
#include "apdu/dispatcher.h"

global_ctx_t G_context;

const internal_storage_t N_storage_real;

/**
 * Handle APDU command received and send back APDU response using handlers.
 */
void app_main() {
	// Length of APDU command received in G_io_apdu_buffer
	int input_len = 0;
	// Structured APDU command
	command_t cmd;

	io_init();

	//ui_menu_main();

	// Reset context
	explicit_bzero(&G_context, sizeof(G_context));

	// Initialize the NVM data if required
	if (N_storage.initialized != 0x01) {
		internal_storage_t storage;
		storage.blind_signed_allowed = 0x00;
		storage.initialized = 0x01;
		nvm_write((void *) &N_storage, &storage, sizeof(internal_storage_t));
	}

	for (;;) {
		BEGIN_TRY {
			TRY {
				// Receive command bytes in G_io_apdu_buffer
				if ((input_len = io_recv_command()) < 0) {
					PRINTF("=> io_recv_command failure\n");
					return;
				}

				// Parse APDU command from G_io_apdu_buffer
				if (!apdu_parser(&cmd, G_io_apdu_buffer, input_len)) {
					PRINTF("=> /!\\ BAD LENGTH: %.*H\n", input_len, G_io_apdu_buffer);
					io_send_sw(SW_WRONG_DATA_LENGTH);
					continue;
				}

				PRINTF("=> CLA=%02X | INS=%02X | P1=%02X | P2=%02X | Lc=%02X | CData=%.*H\n",
						cmd.cla,
						cmd.ins,
						cmd.p1,
						cmd.p2,
						cmd.lc,
						cmd.lc,
						cmd.data);

				// Dispatch structured APDU command to handler
				if (apdu_dispatcher(&cmd) < 0) {
					PRINTF("=> apdu_dispatcher failure\n");
					return;
				}
			}
			CATCH(EXCEPTION_IO_RESET) {
				THROW(EXCEPTION_IO_RESET);
			}
			CATCH_OTHER(e) {
				io_send_sw(e);
			}
			FINALLY {
			}
			END_TRY;
		}
	}
}

/**
 * Exit the application and go back to the dashboard.
 */
void app_exit() {
    BEGIN_TRY_L(exit) {
        TRY_L(exit) {
            os_sched_exit(-1);
        }
        FINALLY_L(exit) {
        }
    }
    END_TRY_L(exit);
}

/**
 * Main loop to setup USB, Bluetooth, UI and launch app_main().
 */
__attribute__((section(".boot"))) int main() {
    __asm volatile("cpsie i");

    os_boot();

    for (;;) {
        // Reset UI
        memset(&G_ux, 0, sizeof(G_ux));

        BEGIN_TRY {
            TRY {
                io_seproxyhal_init();

#ifdef HAVE_BLE
                G_io_app.plane_mode = os_setting_get(OS_SETTING_PLANEMODE, NULL, 0);
#endif  // HAVE_BLE

                USB_power(0);
                USB_power(1);

                ui_menu_main();

#ifdef HAVE_BLE
                BLE_power(0, NULL);
                BLE_power(1, "Nano X");
#endif  // HAVE_BLE
                app_main();
            }
            CATCH(EXCEPTION_IO_RESET) {
                CLOSE_TRY;
                continue;
            }
            CATCH_ALL {
                CLOSE_TRY;
                break;
            }
            FINALLY {
            }
        }
        END_TRY;
    }

    app_exit();

    return 0;
}