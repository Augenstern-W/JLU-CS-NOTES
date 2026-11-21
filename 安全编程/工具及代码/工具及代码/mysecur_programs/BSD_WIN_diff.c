1	/*
2	 * winsock1.c
5	 */	
7	#ifdef WIN32	
9	#pragma comment(lib, "ws2_32.lib")	/* required for winsock */	
11	#include <winsock2.h>	
13	#else	
15	/* UNIX-specific header files includes */	
17	#endif
18	
19	#include <stdio.h>	
21	int
22	main(void)
23	{
24	#ifdef WIN32
25		WSADATA wsa;
26		/* additional Win32-specific variables */
27	#else
28		/* UNIX-specific variables */
29	#endif
30	
31	#ifdef WIN32
32		/* initialize winsock */
33		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
34		{
35			printf("WSAStartup() failed.\n");
36			return(1);
37		}
38	#endif
39		
40		/* 
41		 * ready to use sockets API 
42		 */
43	
44		return(0);
45	}

1		/* handle return value as integer */
2		ret     = setsockopt(sd, IPPROTO_IP, IP_HDRINCL, 
3	                            (const char *) &flg, sizeof(flg));
4		/* check if values is less than zero */
5		if(ret  < 0)
6		{
7			printf("setsockopt() failed.\n");
8			return(1);
9		}
10	
11		/* handle return value using ifdefs */
12		ret     = setsockopt(sd, IPPROTO_IP, IP_HDRINCL, 
13	                            (const char *) &flg, sizeof(flg));
14		/* if Win32, check for SOCKET_ERROR constant */
15	#ifdef WIN32
16		if(ret == SOCKET_ERROR)
17	#else
18		/* otherwise, check for value less than zero */
19		if(ret  < 0)
20	#endif
21		{
22			printf("setsockopt() failed.\n");
23			return(1);
24		}

1	/*
2	 * error1.c
5	 */
6	
7	#ifdef WIN32
8	
9	#pragma comment(lib, "ws2_32.lib")
10	#include <winsock2.h>
11	
12	#else
13	
14	#include <sys/types.h>
15	#include <sys/socket.h>
16	
17	/* required for errno */
18	#include <errno.h>	
19	
20	#endif
21	
22	#include <stdio.h>
23	
24	int
25	main(void)
26	{
27	#ifdef WIN32
28		WSADATA wsa;
29	#endif
31		int sd  = 0;
32		int num = 0;
34	/* intialize Winsock if on Win32 */
35	#ifdef WIN32
36		memset(&wsa, 0x0, sizeof(WSADATA));
38		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
39		{
40			printf("WSAStartup() failed.\n");
41			return(1);
42		}
43	#endif
45		sd     = (int) socket(AF_INET, SOCK_STREAM, 0);
46		/* get extended error information using WSAGetLastError() */
47	#ifdef WIN32
48		if(sd == INVALID_SOCKET)
49		{
50			num = WSAGetLastError();
51	#else
52		/* get extended error information using errno */
53		if(sd  < 0)
54		{
55			num = errno;
56	#endif
57			printf("error code #%d\n", num);
58			return(1);
59		}
61		return(0);
62	}

/*
2	 * socket1.c*
4	 * cross-platform compatible example
5	 * of socket() function.
6	 */
8	#ifdef WIN32	
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")	
13	#include <winsock2.h>	
15	#else
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
21	#endif
22	
23	/* required for printf() */
24	#include <stdio.h>
25	
26	int
27	main(void)
28	{
29	#ifdef WIN32
30		WSADATA wsa;	/* used by WSAStartup() */
31		SOCKET  sd = 0;
32	#else
33		int     sd = 0;
34	#endif
35	
36	/* must initialize winsock if on win32 platform */
37	#ifdef WIN32
38		memset(&wsa, 0x0, sizeof(WSADATA));
39	
40		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
41		{
42			printf("WSAStartup() failed.\n");
43			return(1);
44		}
45	#endif	
47		/* create socket descriptor */
48		sd     = socket(AF_INET, SOCK_STREAM, 0);	
50		/* if Win32, check for INVALID_SOCKET constant */
51	#ifdef WIN32
52		if(sd == INVALID_SOCKET)
53		/* otherwise, check for -1 */
54	#else
55		if(sd  < 0)
56	#endif
57		{
58			printf("socket() failed.\n");
59			return(1);
60		}
61	
62		printf("socket descriptor created.\n");
63	
64		return(0);
65	}

1	/*
2	 * connect1.c
4	 * cross-platform compatible  example  of
5	 * connect() function.
6	 */
7	
8	#ifdef WIN32
9	
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
12	
13	#include <winsock2.h>
14	
15	#else
16	
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
20	#include <netinet/in.h>
21	
22	#endif
23	
24	#include <stdio.h>
25	
26	/* target IP address & port to connect to */
27	#define TARGET_ADDR	"127.0.0.1"
28	#define TARGET_PORT	135
29	
30	int
31	main(void)
32	{
33	#ifdef WIN32
34		WSADATA wsa;	/* used by WSAStartup() */
35		SOCKET  sd = 0;
36	#else
37		int     sd = 0;
38	#endif
40		struct sockaddr_in sin ;
41		int                ret = 0;	
43	/* must initialize winsock if on win32 platform */
44	#ifdef WIN32
45		memset(&wsa, 0x0, sizeof(WSADATA));
47		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
48		{
49			printf("WSAStartup() failed.\n");
50			return(1);
51		}
52	#endif	
54		/* create TCP socket */
55		sd     = socket(AF_INET, SOCK_STREAM, 0);
56		/* if Win32, check for INVALID_SOCKET constant */
57	#ifdef WIN32
58		if(sd == INVALID_SOCKET)
59		/* otherwise, check for -1 */
60	#else
61		if(sd  < 0)
62	#endif
63		{
64			printf("socket() failed.\n");
65			return(1);
66		}

68		printf("socket descriptor created.\n");
69	
70		/* connect socket to remote host/port */
71		memset(&sin, 0x0, sizeof(sin));
72	
73		sin.sin_family      = AF_INET;
74	
75	   /* destination port */
76		sin.sin_port        = htons(TARGET_PORT);
77	
78	   /* destination IP address */
79		sin.sin_addr.s_addr = inet_addr(TARGET_ADDR);
80	
81		ret = connect(sd, (struct sockaddr *) &sin, sizeof(sin));
82		/* if Win32, check for SOCKET_ERROR constant */
83	#ifdef WIN32
84		if(ret == SOCKET_ERROR)
85		/* otherwise, check for value less than zero */
86	#else
87		if(ret  < 0)
88	#endif
89		{
90			printf("connect() failed.\n");
91			return(1);
92		}
93	
94		return(0);
95	}

1	/*
2	 * bind1.c
3	 *
4	 * cross-platform compatible  example  of
5	 * bind() function.
6	 */
7	
8	#ifdef WIN32
9	
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
12	
13	#include <winsock2.h>
14	
15	#else
16	
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
20	#include <netinet/in.h>
21	
22	#endif
23	
24	#include <stdio.h>
25	
26	/* local port to bind to */
27	#define	LOCAL_PORT	1234
28	

29	int
30	main(void)
31	{
32	#ifdef WIN32
33		WSADATA wsa;	/* used by WSAStartup() */
34		SOCKET  sd = 0;
35	#else
36		int     sd = 0;
37	#endif
38	
39		struct sockaddr_in sin ;
40		int	 ret = 0;	
42	/* must initialize winsock if on win32 platform */
43	#ifdef WIN32
44		memset(&wsa, 0x0, sizeof(WSADATA));	
46		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
47		{
48			printf("WSAStartup() failed.\n");
49			return(1);
50		}
51	#endif	
53		/* create UDP socket */
54		sd     = socket(AF_INET, SOCK_DGRAM, 0);
55		/* if Win32, check for INVALID_SOCKET constant */
56	#ifdef WIN32
57		if(sd == INVALID_SOCKET)
58		/* otherwise, check for -1 */
59	#else
60		if(sd  < 0)
61	#endif
62		{
63			printf("socket() failed.\n");
64			return(1);
65		}

67		printf("socket descriptor created.\n");
68	
69		/* bind socket to local port */
70	
71		memset(&sin, 0x0, sizeof(sin));
72	
73		sin.sin_family        = AF_INET;
74	
75		/* port to bind to */
76		sin.sin_port	    = htons(LOCAL_PORT);
77	
78		/* make available on all interfaces/addresses */
79		sin.sin_addr.s_addr   = INADDR_ANY;
80	
81		/* bind socket */
82		ret = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
83	#ifdef WIN32
84		if(ret == SOCKET_ERROR)
85	#else
86		if(ret  < 0)
87	#endif
88		{
89			printf("bind() failed.\n");
90			return(1);
91		}
92	
93		return(0);
94	}

1	/*
2	 * listen1.c
3	 *
4	 * cross-platform compatible  example  of
5	 * listen() function.
6	 */
7	
8	#ifdef WIN32
9	
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
12	
13	#include <winsock2.h>
14	
15	#else
16	
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
20	#include <netinet/in.h>
21	
22	#endif
23	
24	#include <stdio.h>
25	
26	/* local port to bind to */
27	#define	LOCAL_PORT	1234
28	#define	BACKLOG		10

30	int
31	main(void)
32	{
33	#ifdef WIN32
34		WSADATA wsa;	/* used by WSAStartup() */
35		SOCKET  sd = 0;
36	#else
37		int     sd = 0;
38	#endif
39	
40		struct sockaddr_in sin ;
41		int	               ret = 0;
42	
43	/* must initialize winsock if on win32 platform */
44	#ifdef WIN32
45		memset(&wsa, 0x0, sizeof(WSADATA));
46	
47		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
48		{
49			printf("WSAStartup() failed.\n");
50			return(1);
51		}
52	#endif
53	
54		/* create TCP socket */
55		sd     = socket(AF_INET, SOCK_STREAM, 0);

56		/* if Win32, check for INVALID_SOCKET constant */
57	#ifdef WIN32
58		if(sd == INVALID_SOCKET)
59		/* otherwise, check for -1 */
60	#else
61		if(sd  < 0)
62	#endif
63		{
64			printf("socket() failed.\n");
65			return(1);
66		}
67	
68		printf("socket descriptor created.\n");
69	
70		/* bind socket to local port */
71	
72		memset(&sin, 0x0, sizeof(sin));
73	
74		sin.sin_family        = AF_INET;
75	
76		/* port to bind to */
77		sin.sin_port	    = htons(LOCAL_PORT);
78	
79		/* make available via all interfaces */
80		sin.sin_addr.s_addr   = INADDR_ANY;
81	

82		/* bind socket */
83		ret = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
84	#ifdef WIN32
85		if(ret == SOCKET_ERROR)
86	#else
87		if(ret  < 0)
88	#endif
89		{
90			printf("bind() failed.\n");
91			return(1);
92		}	
94		printf("socket bound!\n");	
96		/* set backlog using listen() function,
97	      set socket into listening state */
98		ret = listen(sd, BACKLOG);
99	#ifdef WIN32
100		if(ret == SOCKET_ERROR)
101	#else
102		if(ret  < 0)
103	#endif
104		{
105			printf("listen() failed.\n");
106			return(1);
107		}
109		printf("listen() ok!\n");
111		return(0);
112	}

1	/*
2	 * accept1.c
4	 * cross-platform compatible  example  of
5	 * accept() function.
6	 */
7	
8	#ifdef WIN32
9	
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
12	
13	#include <winsock2.h>
14	
15	#else
16	
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
20	#include <netinet/in.h>
21	
22	#endif
23	
24	#include <stdio.h>
25	
26	/* local port to bind to */
27	#define	LOCAL_PORT	1234
28	#define	BACKLOG		10
29	
30	int
31	main(void)
32	{
33	#ifdef WIN32
34		WSADATA wsa;	/* used by WSAStartup() */
35		SOCKET  sd = 0;
36		SOCKET  cl = 0;	/* client socket */
37	#else
38		int     sd = 0;
39		int     cl = 0;	/* client socket */
40	#endif
42		struct sockaddr_in sin ;
43		int	 len = sizeof(sin); /* required for accept() */
44		int	 ret = 0;
45	
46	/* must initialize winsock if on win32 platform */
47	#ifdef WIN32
48		memset(&wsa, 0x0, sizeof(WSADATA));
49	
50		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
51		{
52			printf("WSAStartup() failed.\n");
53			return(1);
54		}
55	#endif
56	
57		/* create TCP socket */
58		sd     = socket(AF_INET, SOCK_STREAM, 0);
59		/* if Win32, check for INVALID_SOCKET constant */
60	#ifdef WIN32
61		if(sd == INVALID_SOCKET)
62		/* otherwise, check for -1 */
63	#else
64		if(sd  < 0)
65	#endif
66		{
67			printf("socket() failed.\n");
68			return(1);
69		}	
71		printf("socket descriptor created.\n");	
73		/* bind socket to local port */	
75		memset(&sin, 0x0, sizeof(sin));	
77		sin.sin_family        = AF_INET;	
79		/* port to bind to */
80		sin.sin_port	= htons(LOCAL_PORT);	
82		/* make available via all interfaces */
83		sin.sin_addr.s_addr   = INADDR_ANY;
85		/* bind socket */
86		ret = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
87	#ifdef WIN32
88		if(ret == SOCKET_ERROR)
89	#else
90		if(ret  < 0)
91	#endif
92		{
93			printf("bind() failed.\n");
94			return(1);
95		}
97		printf("socket bound!\n");	
99		/* set backlog using listen() function */
100		ret = listen(sd, BACKLOG);
101	#ifdef WIN32
102		if(ret == SOCKET_ERROR)
103	#else
104		if(ret  < 0)
105	#endif
106		{
107			printf("listen() failed.\n");
108			return(1);
109		}	
111		printf("listen() ok!\n");
113		cl     = accept(sd, (struct sockaddr *) &sin, &len);
114	#ifdef WIN32
115		if(cl == SOCKET_ERROR)
116	#else
117		if(cl  < 0)
118	#endif
119		{
120			printf("accept() failed.\n");
121			return(1);
122		}
124		printf("connection received.\n");	
126		return(0);
127	}

1	/*
2	 * select1.c
4	 * cross-platform compatible  example  of
5	 * select() function.
6	 */
8	#ifdef WIN32
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
13	#include <winsock2.h>
15	#else
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
20	#include <netinet/in.h>
21	#include <sys/time.h>	
23	#endif
24	
25	#include <stdio.h>
26	
27	/* local port to bind to */
28	#define	LOCAL_PORT	1234
29	
30	/* receive buffer length */
31	#define	BUF_LEN		1024
32	

33	int
34	main(void)
35	{
36	#ifdef WIN32
37		WSADATA wsa;	/* used by WSAStartup() */
38		SOCKET  sd = 0;
39	#else
40		int     sd = 0;
41	#endif	
43		struct sockaddr_in sin  ;
44		struct timeval     tv   ;	/* required for select() timeout */
45		fd_set	 fdset;	/* required for select() function*/
46		char	buf[BUF_LEN];
47		int	 ret = 0;	
49	/* must initialize winsock if on win32 platform */
50	#ifdef WIN32
51		memset(&wsa, 0x0, sizeof(WSADATA));
52	
53		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
54		{
55			printf("WSAStartup() failed.\n");
56			return(1);
57		}
58	#endif	
60		/* create UDP socket */
61		sd     = socket(AF_INET, SOCK_DGRAM, 0);

62		/* if Win32, check for INVALID_SOCKET constant */
63	#ifdef WIN32
64		if(sd == INVALID_SOCKET)
65		/* otherwise, check for -1 */
66	#else
67		if(sd  < 0)
68	#endif
69		{
70			printf("socket() failed.\n");
71			return(1);
72		}
74		printf("socket descriptor created.\n");
76		/* bind socket to local port */
78		memset(&sin, 0x0, sizeof(sin));
80		sin.sin_family        = AF_INET;
82		/* port to bind to */
83		sin.sin_port          = htons(LOCAL_PORT);
85		/* make available via all interfaces */
86		sin.sin_addr.s_addr   = INADDR_ANY;
88		/* bind socket */
89		ret = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
90	#ifdef WIN32
91		if(ret == SOCKET_ERROR)
92	#else
93		if(ret  < 0)
94	#endif
95		{
96			printf("bind() failed.\n");
97			return(1);
98		}
100		/* use select function to test when socket descriptor is ready for reading */
102		memset(&fdset, 0x0, sizeof(fd_set));
104		FD_SET(sd, &fdset);
106		memset(&tv, 0x0, sizeof(struct timeval));
108		tv.tv_sec = 5;
110		/* the Winsock version of  the select  function
111		   ignores the first argument, nfds, so a value
112		   of zero is passed to avoid compilation warning */
113	#ifdef WIN32
114		ret = select(0, &fdset, NULL, NULL, &tv);
116		/* the BSD sockets version of the select function
117		   requires the nfds argument, so we provide it */
118	#else
120		ret = select(sd + 1, &fdset, NULL, NULL, &tv);
121	#endif
123		/* if win32, check for SOCKET_ERROR value */
124	#ifdef WIN32
125		if(ret == SOCKET_ERROR)
126		/* otherwise, check for value less than zero */
127	#else
128		if(ret  < 0)
129	#endif
130		{
131			printf("select() failed.\n");
132			return(1);
133		}
134		/* if ret is zero, the tv.tv_sec timeout has passed */
135		else if(ret == 0)
136		{
137			printf("select() timeout.\n");
138			return(1);
139		}
140		
141		/* data ready for reading.. */
142	
143		/* receive UDP datagram via recv() function */
144		ret     = recv  (sd, (char *) buf, BUF_LEN, 0);
145	#ifdef WIN32
146		if(ret == SOCKET_ERROR)
147	#else
148		if(ret  < 0)
149	#endif
150		{
151			printf("recv() failed.\n");
152			return(1);
153		}
154		
155		printf("recv ok.\n");
156	
157		return(0);
158	}

1	/*
2	 * sendto1.c
4	 * cross-platform  compatible  example  of
5	 * sendto() function. send UDP datagram to
6	 * port 1234 at address 127.0.0.1
7	 */
9	#ifdef WIN32
11	/* required for winsock */
12	#pragma comment(lib, "ws2_32.lib")
14	#include <winsock2.h>
16	#else
18	/* UNIX header files */
19	#include <sys/types.h>
20	#include <sys/socket.h>
21	#include <netinet/in.h>
22	#include <arpa/inet.h>
24	#endif
25	
26	#include <stdio.h>
27	
28	/* target IP address & port to connect to */
29	#define TARGET_ADDR	"127.0.0.1"
30	#define TARGET_PORT	1234
31	
32	/* example data to send */
33	struct data
34	{
35		int x;
36		int y;
37	};
39	int
40	main(void)
41	{
42	#ifdef WIN32
43		WSADATA wsa;	/* used by WSAStartup() */
44		SOCKET  sd = 0;
45	#else
46		int     sd = 0;
47	#endif
49		struct sockaddr_in sin ;
50		struct data        data;
51		int                ret = 0;
53	/* must initialize winsock if on win32 platform */
54	#ifdef WIN32
55		memset(&wsa, 0x0, sizeof(WSADATA));
57		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
58		{
59			printf("WSAStartup() failed.\n");
60			return(1);
61		}
62	#endif
64		/* create UDP socket */
65		sd     = socket(AF_INET, SOCK_DGRAM, 0);
66		/* if Win32, check for INVALID_SOCKET constant */
67	#ifdef WIN32
68		if(sd == INVALID_SOCKET)
69		/* otherwise, check for -1 */
70	#else
71		if(sd  < 0)
72	#endif
73		{
74			printf("socket() failed.\n");
75			return(1);
76		}
78		printf("socket descriptor created.\n");
80		/* define remote end-point */
81		memset(&sin, 0x0, sizeof(sin));
83		sin.sin_family      = AF_INET;
84		sin.sin_port        = htons(TARGET_PORT);
85		sin.sin_addr.s_addr = inet_addr(TARGET_ADDR);
87		ret     = connect(sd, (struct sockaddr *) &sin, sizeof(sin));
88	#ifdef WIN32
89		if(ret == SOCKET_ERROR)
90	#else
91		if(ret  < 0)
92	#endif
93		{
94			printf("connect() failed.\n");
95			return(1);
96		}
98		/* send data using send function */
99		data.x = 0;
100		data.y = 0;
101		
102		/* cast pointer from type struct data * to const char *
103		   to avoid compiler warnings w/ Visual Studio */
104		ret     = send(sd, (const char *) &data, sizeof(data), 0);
105	#ifdef WIN32
106		if(ret == SOCKET_ERROR)
107	#else
108		if(ret  < 0)
109	#endif
110		{
111			printf("send() failed.\n");
112			return(1);
113		}
114	
115		printf("data sent.\n");
116	
117		return(0);
118	}
1	/*
2	 * recv1.c
4	 * cross-platform compatible  example  of
5	 * recv() function.
6	 */
8	#ifdef WIN32
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
12	
13	#include <winsock2.h>
14	
15	#else
16	
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
20	#include <netinet/in.h>
21	
22	#endif
23	
24	#include <stdio.h>
25	
26	/* local port to bind to */
27	#define	LOCAL_PORT	1234
28	
29	/* receive buffer length */
30	#define	BUF_LEN		1024
31	
32	int
33	main(void)
34	{
35	#ifdef WIN32
36		WSADATA wsa;	/* used by WSAStartup() */
37		SOCKET  sd = 0;
38	#else
39		int     sd = 0;
40	#endif
41	
42		struct sockaddr_in sin ;
43		char	 buf[BUF_LEN];
44		int	 ret = 0;
45	
46	/* must initialize winsock if on win32 platform */
47	#ifdef WIN32
48		memset(&wsa, 0x0, sizeof(WSADATA));
49	
50		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
51		{
52			printf("WSAStartup() failed.\n");
53			return(1);
54		}
55	#endif
56	
57		/* create UDP socket */
58		sd     = socket(AF_INET, SOCK_DGRAM, 0);
59		/* if Win32, check for INVALID_SOCKET constant */
60	#ifdef WIN32
61		if(sd == INVALID_SOCKET)
62		/* otherwise, check for -1 */
63	#else
64		if(sd  < 0)
65	#endif
66		{
67			printf("socket() failed.\n");
68			return(1);
69		}
71		printf("socket descriptor created.\n");
73		/* bind socket to local port */
75		memset(&sin, 0x0, sizeof(sin));
77		sin.sin_family        = AF_INET;
79		/* port to bind to */
80		sin.sin_port		  = htons(LOCAL_PORT);
82		/* make available via all interfaces */
83		sin.sin_addr.s_addr   = INADDR_ANY;
85		/* bind socket */
86		ret = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
87	#ifdef WIN32
88		if(ret == SOCKET_ERROR)
89	#else
90		if(ret  < 0)
91	#endif
92		{
93			printf("bind() failed.\n");
94			return(1);
95		}
97	   	printf(waiting for intput.\n?;
98	
99		/* receive UDP datagram via recv() function */
100		ret     = recv  (sd, (char *) buf, BUF_LEN, 0);
101	#ifdef WIN32
102		if(ret == SOCKET_ERROR)
103	#else
104		if(ret  < 0)
105	#endif
106		{
107			printf("recv() failed.\n");
108			return(1);
109		}
110		
111		printf("recv ok.\n");
112	
113		return(0);
114	}
1	/*
2	 * close1.c
4	 * cross-platform  compatible  example of close()/closesocket() functions.
6	 */
8	#ifdef WIN32
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
13	#include <winsock2.h>
15	#else
17	#include <sys/types.h>
18	#include <sys/socket.h>
19	#include <unistd.h>
21	#endif
22	
23	#include <stdio.h>
24	
25	int
26	main(void)
27	{
28	#ifdef WIN32
29		WSADATA wsa;	/* required for WSAStartup() */
30		SOCKET  sd = 0;
31	#else
32		int    sd = 0;
33	#endif
34	
35	/* initialize winsock if on win32 */
36	#ifdef WIN32
37		memset(&wsa, 0x0, sizeof(WSADATA));
39		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
40		{
41			printf("WSAStartup() failed.\n");
42			return(1);
43		}
44	#endif
46		/* create socket descriptor */
47		sd = socket(AF_INET, SOCK_STREAM, 0);
48		/* if win32, check for INVALID_SOCKET */
49	#ifdef WIN32
50		if(sd == INVALID_SOCKET)
51	#else
52		/* otherwise, check for return val < 0 */
53		if(sd  < 0)
54	#endif
55		{
56			printf("socket() failed.\n");
57			return(1);
58		}
60		/* close socket! */
61	#ifdef WIN32
62		/* win32 specific closesocket call */
63		closesocket(sd);
64	#else
65		/* UNIX close() system call */
66		close(sd);
67	#endif
69		return(0);
70	}

1	/*
2	 * setsockopt1.c
3	 *
4	 * cross-platform compatible  example of
5	 * TCP raw socket creation  and  use  of
6	 * setsockopt function to set IP_HDRINCL
7	 * option.
8	 */
9	
10	#ifdef WIN32
11	
12	/* required for winsock */
13	#pragma comment(lib, "ws2_32.lib")
14	
15	#include <winsock2.h>
16	#include <ws2tcpip.h>	/* required for IP_HDRINCL option */
17	
18	#else
19	
20	/* UNIX header files */
21	#include <sys/types.h>
22	#include <sys/socket.h>
23	#include <netinet/in.h>
24	
25	#endif
26	
27	#include <stdio.h>
28	
29	int
30	main(void)
31	{
32	#ifdef WIN32
33		WSADATA wsa;	/* used by WSAStartup() */
34		SOCKET  sd  = 0;
35	#else
36		int     sd  = 0;
37	#endif	
39		int     flg = 1;
40		int     ret = 0;
41	
42	/* must initialize winsock if on win32 platform */
43	#ifdef WIN32
44		memset(&wsa, 0x0, sizeof(WSADATA));
45	
46		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
47		{
48			printf("WSAStartup() failed.\n");
49			return(1);
50		}
51	#endif
52	
53		/* create TCP raw socket */
54		sd     = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
55		/* if Win32, check for INVALID_SOCKET constant */
56	#ifdef WIN32
57		if(sd == INVALID_SOCKET)
58		/* otherwise, check for -1 */
59	#else
60		if(sd  < 0)
61	#endif
62		{
63			printf("socket() failed.\n");
64			return(1);
65		}
67		printf("socket descriptor created.\n");
68	
69		ret     = setsockopt(sd, IPPROTO_IP, IP_HDRINCL, 
70	                           (const char *) &flg, sizeof(flg));
71		/* if Win32, check for SOCKET_ERROR constant */
72	#ifdef WIN32
73		if(ret == SOCKET_ERROR)
74		/* otherwise, check for -1 */
75	#else
76		if(ret  < 0)
77	#endif
78		{
79			printf("setsockopt() failed.\n");
80			return(1);
81		}
82	
83		printf("IP_HDRINCL socket option set.\n");
84	
85		return(0);
86	}
1	/*
2	 * ioctl1.c
3	 *
4	 * cross-platform example of
5	 * ioctl()/ioctlsocket() functions.
6	 */
7	
8	#ifdef WIN32
9	
10	/* required for winsock */
11	#pragma comment(lib, "ws2_32.lib")
12	
13	#include <winsock2.h>
14	
15	#else
16	
17	/* UNIX header files */
18	#include <sys/types.h>
19	#include <sys/socket.h>
20	
21	/* required for ioctl() */
22	#include <sys/ioctl.h>		
23	
24	#endif
25	
26	#include <stdio.h>
27	
28	int
29	main(void)
30	{
31	#ifdef WIN32
32		WSADATA       wsa;		/* used by WSAStartup() */
33		SOCKET        sd  = 0;
34		unsigned long val = 1;		/* used for ioctlsocket() */
35	#else
36		int           sd  = 0;
37		long          val = 1;		/* used for ioctl() */
38	#endif
39	
40		int     ret = 0;			/* ioctl/ioctlsocket return val */
41	
42	/* must initialize winsock if on win32 platform */
43	#ifdef WIN32
44		memset(&wsa, 0x0, sizeof(WSADATA));
45	
46		if(WSAStartup(MAKEWORD(2, 0), &wsa) != 0x0)
47		{
48			printf("WSAStartup() failed.\n");
49			return(1);
50		}
51	#endif
52	
53		/* create TCP socket */
54		sd     = socket(AF_INET, SOCK_STREAM, 0);
55		/* if Win32, check for INVALID_SOCKET constant */
56	#ifdef WIN32
57		if(sd == INVALID_SOCKET)
58		/* otherwise, check for -1 */
59	#else
60		if(sd  < 0)
61	#endif
62		{
63			printf("socket() failed.\n");
64			return(1);
65		}
67		printf("socket descriptor created.\n");
69	#ifdef WIN32
70		ret     = ioctlsocket(sd, FIONBIO, &val);
71		if(ret == SOCKET_ERROR)
72	#else
73		ret     = ioctl(sd, FIONBIO, &val);
74		if(ret  < 0)
75	#endif
76		{
77			printf("ioctl FIONBIO failed.\n");
78			return(1);
79		}
80	
81		printf("ioctl FIONBIO set.\n");
82	
83		return(0);
84	}
