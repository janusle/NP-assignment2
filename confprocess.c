#include "confprocess.h"


void
explode( char* line , char* array[ARRAYLEN] )
{
  int i, size;

  array[0] = (char*)Malloc( sizeof(char) * LEN );
  array[1] = (char*)Malloc( sizeof(char) * LEN );

  size = strlen(line);
  memset( array[0], 0, sizeof(char)*LEN );
  memset( array[1], 0, sizeof(char)*LEN );

  for( i=0; !isspace(line[i]) && i<size ;i++)
  ;

  strncpy( array[0], line, i );

  for(; isspace(line[i]) && i<size; i++)
  ;

  strcpy( array[1], line+i);

}

/*
static void*
Malloc( size_t size )
{
  void *tmp;

  if( (tmp=malloc( size)) == NULL )
  {
     err_quit("Memory Error"); 
  }
  
  return tmp;

}
*/

static void 
trim( char* line )
{
   char *st;
   int size, i;
   
   size = strlen(line);

   /* skip space */
   for(i=0; i<size && isspace(line[i]); i++)
   ;

   /* all the items in the line is space */
   if( i >= size )
    line[0] = '\0';
   
   /* cut the space in the front */
   st = (char*)Malloc( sizeof(char)*LEN); 
   strcpy( st, line+i );
   
   size = strlen(st);
  
   
   for( i = size-1; isspace(st[i]) && i>=0 ; i-- ) 
   ;     
   
   st[i+1] = '\0';
   

   strcpy(line, st);

   free(st);
}


static char* 
getl( char *line, FILE* fd )
{
   char c; 
   int i;
    
   i=0;
   while( ( ( c = fgetc(fd)) != '\n' &&  c != '\r' ) && c != EOF )
   {
      line[i++] = c;
   }

   if( c == EOF )
     return NULL;

   line[i] = '\0';
  
   trim( line );    

   return line;
}


static int 
istype( char* type )
{
  if( strlen( type ) <= PREFIX )
    return false;
  
  if( strncmp( type, "type-", 5 ) != 0 )
    return false;

  return true;
}


void
init_config( char* filename , char config[][CONFSIZE], contenttyp* type[TYPNUM] )
{
  FILE* fd;
  char *line, *tmp[ARRAYLEN];
  int i;
   
  line = (char*)Malloc( sizeof(char) * LINENUM);

  fd = fopen(filename, "r");
  if( fd == NULL )
  {
     err_quit("Read File Error"); 
  }

  for(i=0; i<CONFLEN; i++)
    strcpy( config[i], "" );
  
  for(i=0; i<TYPNUM; i++ )
    type[i] = NULL;


  i=0;
  while( (getl(line,  fd ) != NULL) )
  {

    if( isalpha(line[0]) )
    {

      explode( line , tmp );
      
      if( strcmp( "port", tmp[OPTION] ) == 0 )
      {
         strcpy( config[PORT], tmp[VALUE] );
      }

      if( strcmp( "host", tmp[OPTION] ) == 0 )
      { 
        strcpy( config[HOST], tmp[VALUE] );
      }

      if( strcmp( "root", tmp[OPTION] ) == 0 )
      { 
        strcpy( config[ROOT], tmp[VALUE] );
      }
     
      if( strcmp( "shutdown-signal" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[SDSIG], tmp[VALUE] );
      }

      if( strcmp( "shutdown-file" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[SDFILE], tmp[VALUE] );
      }
 
      if( strcmp( "status-request" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[STAT], tmp[VALUE] );
      }

      if( strcmp( "logfile" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[LOG], tmp[VALUE] );
      }
 
      if( strcmp( "recordfile" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[RD], tmp[VALUE] );
      }

      if( strcmp( "logging" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[LOGGING], tmp[VALUE] );
      }

      if( strcmp( "recording" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[RECORDING], tmp[VALUE] );
      }

      if( strcmp( "type" , tmp[OPTION] ) == 0 )
      {
        strcpy( config[DEFAULT], tmp[VALUE] );
      }
      
      if( istype( tmp[OPTION] ) )
      {
        type[i] = (contenttyp*)Malloc( sizeof(contenttyp) );
        strcpy( type[i]->ext, tmp[OPTION]+PREFIX );
        strcpy( type[i++]->contype, tmp[VALUE] );
      }

    }
  }
  
  /* set default value */
  /*
  if( strcmp( config[PORT], "" ) == 0  || 
      strcmp( config[HOST], "" ) == 0  ||
      strcmp( config[ROOT], "" ) == 0 ||
      strcmp( config[ )  )
  {
    fprintf(stderr, "Bad format in config file\n");
    exit(EXIT_FAILURE);
  }


  if( config[PRINT_MSG] == NULL )
  {
    config[PRINT_MSG] = (char*)Malloc( sizeof(char)*LEN );
    strcpy( config[PRINT_MSG], "ON");
  }


  if( config[SUP_TIMEOUT] == NULL )
  {
    config[SUP_TIMEOUT] = (char*)Malloc( sizeof(char)*LEN );
    strcpy( config[SUP_TIMEOUT], "ON");
  }
  */
  free(line);
  fclose(fd); 
}

