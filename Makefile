all: gethostname getaddrinfo udp_sendto udp_recvfrom getnameinfo tcp_conn tcp_sigpipe udp_sv tcp_sv tcp_busy tcp_conc

gethostname: gethostname.c
	gcc gethostname.c -o gethostname

getaddrinfo: getaddrinfo.c
	gcc getaddrinfo.c -o getaddrinfo

udp_sendto: udp_sendto.c
	gcc udp_sendto.c -o udp_sendto

udp_recvfrom: udp_recvfrom.c
	gcc udp_recvfrom.c -o udp_recvfrom

getnameinfo: getnameinfo.c
	gcc getnameinfo.c -o getnameinfo

tcp_conn: tcp_conn.c
	gcc tcp_conn.c -o tcp_conn

tcp_sigpipe: tcp_sigpipe.c
	gcc tcp_sigpipe.c -o tcp_sigpipe

udp_sv: udp_sv.c
	gcc udp_sv.c -o udp_sv

tcp_sv: tcp_sv.c
	gcc tcp_sv.c -o tcp_sv

tcp_busy: tcp_busy.c
	gcc tcp_busy.c -o tcp_busy

tcp_conc: tcp_conc.c
	gcc tcp_conc.c -o tcp_conc

clean:
	rm -f gethostname getaddrinfo udp_sendto udp_recvfrom getnameinfo tcp_conn tcp_sigpipe udp_sv tcp_sv tcp_busy tcp_conc