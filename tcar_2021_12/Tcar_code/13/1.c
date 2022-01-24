/*
 *	usermain.c (usermain)
 *	User Main
 */
#include <tk/tkernel.h>
#include <tm/tmonitor.h>
#include <utkn/utkn.h>
#include <utkn/coap/coap.h>

const UB SERVER_IP[] = {0x20, 0x01, 0x0d, 0xf7, 0x56, 0x00, 0x80, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x10};

EXPORT INT usermain(void)
{
	static const utkn_6ln_init_t param = {
		.channel = RF_CHANNEL_ANY,
		.scan_type = WPAN_SCAN_ACTIVE,
		.scan_duration = 4,
		.options = 0,
		.tmout = TMO_FEVR,
	};

	udp6_cep_t dst;
	static UB buf[128 + 1] = {0};
	UB rssi;
	udp6_cep_t cep;
	ID cepid;
	ER ercd;
	INT len;

	/* Initialize a normal node. */
	ercd = utkn_6ln_init(&param);
	tm_printf((UB *)"utkn_init: %d\n", ercd);
	cep.addr = ip6_addr_unspec;
	cep.port = htons(5000);
	cepid = udp6_open(&cep);
	tm_printf((UB *)"udp6_open: %d\n", cepid);

	memcpy(dst.addr.addr, SERVER_IP, sizeof(dst.addr.addr));
	dst.port = htons(5000);

	static UB send_message[] = "get";

	/* Send data to a server */
	len = udp6_send(cepid, send_message, strlen(send_message), &dst, TMO_FEVR);
	if (len < 0) {
		tm_printf((UB *)"udp6_send: %d\n", MERCD(len));
	}

	/* Recv data from the server. */
	len = udp6_recv(cepid, buf, sizeof(buf) - 1, &dst, NULL, &rssi, TMO_FEVR);
	if (len < 0) {
		tm_printf((UB *)"udp6_recv: %d\n", MERCD(len));
		return 0;
	}
	tm_printf("Your IP address: %s\n", buf);

	return 0;
}
