/* UPNP IGD Tool v0.1
 * Author : João Paulo Barraca <jpbarraca@av.it.pt>
 * Distributed according to the GNU Public Licence
 *
 * Thanks to Thomas Bernard miniupnp
 * miniupnp is copyrighted by Thomas Bernard
 * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "miniwget.h"
#include "miniupnpc.h"
#include "upnpcommands.h"

#define OP_NONE				0
#define OP_DISCOVER 	1
#define OP_CALLOP			2
#define OP_LIST 			4

static struct option long_options[] = {
	                                      {"list"				, 0, 0, 'l'},
	                                      {"action"			, 1, 0, 'a'},
	                                      {"discover"		, 0, 0, 'd'},
	                                      {"argument"  	, 1, 0, 'p'},
	                                      {"help"				, 0, 0, 'h'},
	                                      {"verbose"		, 0, 0, 'v'},
	                                      {0						, 0, 0, 0}
                                      };

/*
 * Complete an IGDdatas structure with service info
 */
void fillData(struct IGDdatas* data){

	int d,s;
	int ignored = 0;

	for(d=0;d < data->devices;d++){
		struct IGDdevice * dev = data->dlist[d];

		if(!dev)
			continue;

		if(!dev->devicetype[0]){
			ignored++;
			continue;
		}

		for(s=0;s<dev->services;s++)
		{
			struct IGDservice * srv = dev->slist[s];
			if(!srv)
				continue;

			if(!srv->actions)
			{
				char * descXML;
				int descXMLsize = 0;
				char descURL[100];
				sprintf(descURL,"%s:80%s",data->urlbase,srv->scpdurl);
				descXML = miniwget(descURL, &descXMLsize);
				if(descXML){
					parseservicedesc(descXML,descXMLsize, data,d,s);
					free(descXML);
				}
			}
		}
	}
}


/*
 * List all devices, services, actions and arguments
 * If verbose is true, header info is also printed
 */
void listServices( struct IGDdatas *data, const int devid, char verbose) {

	int d,s,a,p;
	int ignored = 0;

	fillData(data);

	for(d=0;d < data->devices;d++){
		struct IGDdevice * dev = data->dlist[d];

		if(!dev || (devid >= 0 && devid != d))
			continue;

		if(!dev->devicetype[0]){
			ignored++;
			continue;
		}

		if(verbose) printf("device type = %s\n", dev->devicetype);
		if(verbose) printf("friendly name = %s\n", dev->friendlyname);
		if(verbose) printf("udn = %s\n", dev->udn);

		for(s=0;s<dev->services;s++)
		{
			struct IGDservice * srv = dev->slist[s];
			if(!srv)
				continue;

			if(verbose) printf("  service type = %s\n", srv->servicetype);
			if(verbose) printf("  control url = %s\n", srv->controlurl);

			for(a=0;a<srv->actions;a++)
			{
				struct IGDaction * act = srv->alist[a];
				if(!act)
					continue;

				printf("     %u:%u:%u\t - %s",d,s,a,act->name);
				int putarg=0;

				for(p=0;p<act->arguments;p++){
					struct IGDargument * arg = act->alist[p];

					if(!arg || !arg->name || !arg->name[0] )
						continue;

					if(!putarg){
						putarg=1;
						int i=0;
						int len = strlen(act->name);

						for(;i<30 - len;i++)
							printf(" ");

						printf("args:");
					}

					printf("%s ",arg->name);
				}
				printf("\n");

			}
			if(verbose) printf("\n");
		}
		if(verbose) printf("\n");
	}
}

/*
 * Prints the Devices contained at given IGDdatas
 */
void listDevices(const struct IGDdatas* data){
	int d=0;

	if(!data)
		return;

	printf("Found Host at: %s\n",data->urlbase);

	printf("Announced devices = %u\n",data->devices);

	for(d=0;d<data->devices;d++)
	{
		if(data->dlist[d] && data->dlist[d]->friendlyname[0]){
			struct IGDdevice *dev = data->dlist[d];
			printf("Device: %u \n",d);
			printf("  friendly name = %s\n",dev->friendlyname);
			printf("  manufacturer = %s\n", dev->manufacturer);
			printf("  model = %s %s \n", dev->modelname,dev->modelnumber);
			printf("\n");
		}else
			printf("Device: %u (bogus)\n",d);

	}
}

/*
 * Preform an UPNP action
 * If parameters are provided they are parsed
 * according to the ordem provided by device
 *
 * Both read and writes are possible
 */
void callUPNPVariable(struct IGDdatas *data, const char* variable, char* arguments, char verbose){

	if(!data || !variable )
		return;

	int d,s,a;
	struct UPNParg * upnpargs = NULL;

	if(sscanf(variable,"%u:%u:%u",&d,&s,&a) != 3)
	{
		printf("Error: Invalid variable (%s)! Please use DEVICE:SERVICE:VARIABLE.\n",variable);
		return;
	}

	fillData(data);

	struct IGDdevice * dev;
	struct IGDservice * srv;
	struct IGDaction * act;

	if(d >= data->devices || !data->dlist[d]){
		printf("Error: Invalid device number %u\n",d);
		return;
	}
	dev = data->dlist[d];

	if(s >= dev->services || !dev->slist[s])
	{
		printf("Error: Invalid service number %u\n",s);
		return;
	}

	srv = dev->slist[s];
	if(!srv->controlurl[0] || !srv->servicetype[0])
	{
		printf("Error: Service has invalid Type and ControlURL\n");
		return;
	}

	if(a >= srv->actions || !srv->alist[a])
	{
		printf("Error: Invalid operation number %u\n",a);
		return;
	}
	act = srv->alist[a];

	char controlURL[1024]; controlURL[0] ='\0';
	sprintf(controlURL,"%s:%u%s",data->urlbase,80,srv->controlurl);
	if(verbose) printf("Getting Variable:\n ControlURL: %s\n ServiceType: %s\n Action: %s\n",controlURL,srv->servicetype,act->name);

	if(arguments && strlen(arguments)){
		
		if(verbose) printf("\nArguments:\n");
		upnpargs = calloc(act->arguments+1,sizeof(struct UPNParg));

		char* val = strsep(&arguments,",");
		int i;
		for(i=0;i<act->arguments;i++)
		{
			upnpargs[i].elt = act->alist[i]->name;
	
			if(!val || val[0]== '\0')
				upnpargs[i].val = NULL;
			else
				upnpargs[i].val = val;
			val =	strsep(&arguments,",");

			if(verbose) printf(" %s -> %s\n",upnpargs[i].elt,upnpargs[i].val);
		}
	}

	if(verbose) printf("\nResult:\n");

	UPNP_GetGeneralVariable(controlURL,srv->servicetype,act->name, upnpargs);

	if(upnpargs)
		free(upnpargs);
}

/*
 * Print usage information
 *
 */
void printUsage(const char* progname){
	printf("Universal Plug and Play IGD Tool v0.1\n  João Paulo Barraca <jpbarraca@av.it.pt>\n\n");
	printf("Usage:\t%s ACTION [-p ARGUMENTS]\n", progname);
	printf("  -d,  --discover\t\t:Find local devices\n");
	printf("  -l,  --list \t\t\t:List devices, services, actions and arguments\n");
	printf("  -a,  --action \t\t:Invoque action. Syntax DEVICE:SERVICE:ACTION\n");
	printf("  -p,  --argument\t\t:Sets optional arguments for action. Syntax: val1,val2,val3...\n");
	printf("  -h,  --help\t\t\t:This help screen\n");
}

int main(int argc, char ** argv)
{
	char * descURL = NULL;
	int option_index = 0;
	int c;
	int devid = -1;
	char arguments[1025]; arguments[0]='\0';
	char action[129]; action[0]='\0';
	char verbose = 0;

	unsigned int operation = OP_NONE;

	if(argc == 1)
	{
		printUsage(argv[0]);
		return -1;
	}

	while(1){
		c = getopt_long(argc,argv,"dlva:p:",long_options,&option_index);

		if(c == -1)
			break;

		switch(c){
		case 'd': operation = OP_DISCOVER; break;
		case 'l': operation = OP_LIST; break;
		case 'a': operation = OP_CALLOP; if(optarg) strncpy(action,optarg,128); break;
		case 'p': if(optarg) strncpy(arguments,optarg,1024); break;
		case 'v': verbose = 1; break;
		default: printUsage(argv[0]); return 0;
		}
	}
	
	if(verbose) printf("Universal Plug and Play IGD Tool v0.1\n  João Paulo Barraca <jpbarraca@av.it.pt>\n\n");
	if(operation == OP_NONE)
		return 0;

	descURL = upnpDiscover(5000);	// timeout = 5secs

	if(descURL)
	{
		struct IGDdatas *data = (struct IGDdatas*) malloc(sizeof(struct IGDdatas));
		struct UPNPUrls urls;

		memset(data, 0, sizeof(struct IGDdatas));
		memset(&urls, 0, sizeof(struct UPNPUrls));

		char * descXML;
		int descXMLsize = 0;
		descXML = miniwget(descURL, &descXMLsize);

		parserootdesc(descXML, descXMLsize, data);

		if(descXML)
		{

			switch(operation)
			{
			case OP_CALLOP:
				callUPNPVariable(data,action,arguments, verbose);
				break;
			case OP_LIST:
				listServices(data,devid,verbose); 
				break;
			case OP_DISCOVER:
				listDevices(data);		
				break;
			default: printf("Error: Action not implemented (yet)!\n"); break;
			}
			free(descXML);
		}
		else
		{
			printf("Error: Cannot get XML description of the device.\n");
		}
		freeIGD(data);
		free(descURL);
	}
	else
	{
		fprintf(stderr, "Error: No IGD UPnP Device found on the network !\n");
	}
	return 0;
}

