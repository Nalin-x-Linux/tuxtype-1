/***************************************************************
 *  manage_xmlLesson.c                                          *
 *                                                              *
 *  Description:  Read XML lesson files generated by            *
 *                Tux4kids-Admin and write result files for it. *                                         *
 *  Author:       Vikas Singh 					*
 *                 vikassingh008@gmail.com ,2010 		*
 *  Copyright:    GPL v3 or later                               *
 *  							    	*
 *  						          	*
 *                                                          	*
 *                                                          	*
 *  TuxMath                                                 	*
 *  Part of "Tux4Kids" Project                              	*
 *  http://tux4kids.alioth.debian.org/                      	*
 ***************************************************************/

#include"globals.h"
#include <libxml/parser.h>
#include <libxml/tree.h>
#include"manage_xmlLesson.h" 
#include"schoolmode.h"
#include"SDL_extras.h"
#include "scripting.h"
#define PATH_MAX 4096

//Local Function prototypes for reading
void parse_cascade(xmlNode *);
void parse_lasergame(xmlNode *);
void parse_phrases(xmlNode *);
void parse_typing_lesson(xmlNode *);
//Local Function prototypes for writing
void write_cascade();
void write_laser();
void write_phrases(int); //for both typing lesson and phrases 

//initialize read and write 
int init_readwrite(char *);
void clean_up();



//variables
  xmlDocPtr doc_read,doc_write;

xmlNodePtr root_write = NULL;  /* node pointer for writing */
    xmlDtdPtr dtd = NULL;  //writing
 xmlNode *root_read = NULL; //reading
int total_no_menus;  
    char buff[256];

  char menu_names[MAX_MENU_ITEMS][MENU_ITEM_LENGTH] = {{'\0'}};



//extern struct result_fish_cascade result; //defined in playgame.c

//char *xml_lesson_path;


int manage_xmlLesson(char *xml_lesson_path)
{
xmlNode *cur_node;
int i;
char fn[PATH_MAX];
char test_file[PATH_MAX]; //this files is used to extract time and date and then deleted
char *write_directory;

//time and date related variables
time_t filetime;
  struct stat filestat;
  struct tm datetime;
  FILE* fp; 


if(init_readwrite(xml_lesson_path)==-1)
return 0;


//input = ( struct input_per_wave *) malloc(MAX_WAVES * sizeof(struct input_per_wave));

 // if (input == NULL)
  //{
  //  printf("Allocation of input to store input values failed");
  //  return 0;
 // }

 for(i=0 , cur_node = root_read->children    ; cur_node != NULL   ;      cur_node = cur_node->next)
  {
    current_game_index=i;
     if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "fish" ) )
     {  
        if(display_screen(i)==-1)    // i highlights the next game to be played
           break;
       i++;   
        parse_cascade(cur_node);

          GenerateWordList(input_cascade.filepath);
          if(input_cascade.level ==1)
           PlayCascade( EASY );
          else 
          if(input_cascade.level ==2)
           PlayCascade( MEDIUM );
          else 
          if(input_cascade.level ==3)
           PlayCascade( HARD );
          else 
          if(input_cascade.level ==4)
           PlayCascade( INSANE );
              input_cascade.level=-1;
 
         write_cascade();
    //   game_score=factoroids_schoolmode(0,current_no_of_waves);
      // write_factors();  
 
     }

   else if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "laser" ) )
     { 
        if(display_screen(i)==-1)    // i highlights the next game to be played
           break;
        i++;         
        parse_lasergame(cur_node);

          GenerateWordList(input_laser.filepath);
          if(input_laser.level ==1)
           PlayLaserGame( EASY );
          else 
          if(input_laser.level ==2)
          PlayLaserGame( MEDIUM );
          else 
          if(input_laser.level ==3)
          PlayLaserGame( HARD );
          else 
          if(input_laser.level ==4)
           PlayLaserGame( INSANE ); 
             input_laser.level=-1;
         write_laser();  
      //  current_no_of_waves=parse_fractions(cur_node);
      // game_score=factoroids_schoolmode(1,current_no_of_waves);
      // write_fractions();   
     }

  else if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "phrases" ) )
     { 
        if(display_screen(i)==-1)    // i highlights the next game to be played
           break;
        i++;   
      parse_phrases(cur_node);
            Phrases(NULL);
      write_phrases(0);     
     }
  else if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "typing_lesson" ) )
     { 
        if(display_screen(i)==-1)    // i highlights the next game to be played
           break;
        i++;   
      parse_typing_lesson(cur_node);
            scripting_schoolmode();
        write_phrases(1);
     }

  }

  // --------------------------------------------------------------------------

 #ifdef BUILD_MINGW32
     write_directory = GetDefaultSaveDir(PROGRAM_NAME);
#else
     write_directory = strdup(getenv("HOME"));
#endif

      //Decide the write directory for writing result  
      write_directory= strdup(getenv("HOME"));




/* FIXME there must be a better method way to do than this    */ 
/*Extract date and time by writing a test file to the same directory*/
/* where we are going to write result and extracting date and time from it */
/*and then deleting that file. */
/* We're going to want to write the date.  Use the filetime  */
    /* rather than calling "time" directly, because "time"       */
    /* returns the time according to whatever computer is        */
    /* running tuxmath, and in a server/client mode it's likely  */
    /* that some of the clients' clocks may be wrong. Use      */
    /* instead the time according to the server on which the     */
    /* accounts are stored, which can be extracted from the      */
    /* modification time of the summary we just wrote.           */
	
snprintf(test_file, PATH_MAX, "%s/testfile",write_directory);
fp=fopen(test_file,"w");
if (fp){
fprintf(fp, "\ntest file");
fclose(fp);
} 
else {fprintf(stderr,"test file not written.\n");}
    
//time and date related code
if (stat(test_file,&filestat) == 0) {
      filetime = filestat.st_mtime;
    } else {
      filetime = time(NULL);
    }
localtime_r(&filetime,&datetime); /* generate broken-down time */

remove(test_file); //got date and time so remove the file






//write the result file
	snprintf(fn, PATH_MAX, "%s/result%d-%d-%d__%d:%d:%d.xml",write_directory,datetime.tm_year+1900, 
              datetime.tm_mon+1, datetime.tm_mday,datetime.tm_hour,datetime.tm_min,datetime.tm_sec);

        if( xmlSaveFormatFileEnc(fn, doc_write, "UTF-8", 1)==-1)
            fprintf(stderr,
                "\nError: couldn't write result file: "
                "%s\n",fn);
        else
         printf("\nResult file saved : %s\n",fn);


clean_up();

/*
* this is to debug memory for regression tests
 */
 xmlMemoryDump();
    return(0);

}


int init_readwrite(char *xml_lesson_path)
{

//code related to reading
xmlNode *cur_node;
  char fn[4096];
//char *lesson_path = "schoolmode/lessonData.xml";
 // snprintf(fn, 4096, "%s/images/%s", DATA_PREFIX, lesson_path);

snprintf(fn, 4096, "%s", xml_lesson_path);

int i;
  //xmlChar *Num_asteroids;
//int serial_number;

  // --------------------------------------------------------------------------
  // Open XML document
  // --------------------------------------------------------------------------

  doc_read = xmlParseFile(fn);

  if (doc_read == NULL) 
        printf("error: could not parse file %s \n",xml_lesson_path);
  else 
        printf("parsed file %s\n",xml_lesson_path);
  // --------------------------------------------------------------------------
  // XML root.
  // --------------------------------------------------------------------------

  /*Get the root element node */
  root_read = xmlDocGetRootElement(doc_read);
  
  // --------------------------------------------------------------------------
  // Must have root element, a name and the name must be "lessonData"
  // --------------------------------------------------------------------------
  
  if( !root_read || 
      !root_read->name ||
      xmlStrcmp(root_read->name,(const xmlChar *)"lessonData") ) 
  {
     xmlFreeDoc(doc_read);
     return -1;
  }

  // --------------------------------------------------------------------------
  // lessonData children: For each factors
  // --------------------------------------------------------------------------


  for(  i=0 , cur_node = root_read->children    ;   cur_node != NULL   ;    cur_node = cur_node->next)
  {
     if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "fish" ) )
     {  
            sprintf(menu_names[i], "%s", cur_node->name); 
            i++;
               //menu_names[i]=(char *)cur_node->name;
     }

   else if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "laser" ) )
     {  
                sprintf(menu_names[i], "%s", cur_node->name); 
                i++;
     }


   else if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "phrases" ) )
     {  
                sprintf(menu_names[i], "%s", cur_node->name); 
                i++;
     }
   else if ( cur_node->type == XML_ELEMENT_NODE  &&
          !xmlStrcmp(cur_node->name, (const xmlChar *) "typing_lesson" ) )
     {  
                sprintf(menu_names[i], "%s", cur_node->name); 
                i++;
     }
  }

total_no_menus=i;

game_completed=(int *)calloc(i,sizeof(i));

//code related to writing

#if !defined(LIBXML_TREE_ENABLED) || !defined(LIBXML_OUTPUT_ENABLED)
fprintf(stderr, "Tree support for writing XML result files not compiled in\n");
    return -1;
#endif
//Macro to check that the libxml version in use is compatible with the version the software has been compiled against
    LIBXML_TEST_VERSION;

    /* 
     * Creates a new document, a node and set it as a root node
     */
    doc_write = xmlNewDoc(BAD_CAST "1.0");
    root_write = xmlNewNode(NULL, BAD_CAST "resultData");
    xmlDocSetRootElement(doc_write, root_write);

    /*
     * Creates a DTD declaration. Isn't mandatory. 
     */
    dtd = xmlCreateIntSubset(doc_write, BAD_CAST "resultData", NULL, BAD_CAST "");
return 0;

}


//parse cascade
void parse_cascade(xmlNode *cur_node)
{
 xmlChar *temp_xml;
 xmlNode *child_node;
  char temp_string[5];

       //printf("Element: %s \n", cur_node->name); 


        for(child_node = cur_node->children; child_node != NULL; child_node = child_node->next)
        {
           if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"words_file") )
           {
             
            
              temp_xml= xmlNodeGetContent(child_node);
              if(temp_xml)
               {
                 //printf("         Wave: %s\n", wave);
                 sprintf(input_cascade.filepath ,"%s", temp_xml); 

               }
              //xmlFree(filename);
           }          
          else if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"level") )   //this level refers to easy/medium/hard
           {
              
               //the level value is got from XML file
                temp_xml= xmlNodeGetContent(child_node);
              if(temp_xml)
               {
                 sprintf(temp_string, "%s", temp_xml); 
                 input_cascade.level=atoi(temp_string);
                   

               }
              //xmlFree(level_xml);
           }          
       else if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"lives") )
           {
              
               //the level value is got from XML file
                temp_xml= xmlNodeGetContent(child_node);
              if(temp_xml)
               {
                 sprintf(temp_string, "%s", temp_xml);  
                 input_cascade.num_of_lives=atoi(temp_string);
                   

               }
              //xmlFree(level_xml);
           }
       else if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"fish_per_level") )
           {
              
               //the level value is got from XML file
                temp_xml= xmlNodeGetContent(child_node);
              if(temp_xml)
               {
                 sprintf(temp_string, "%s", temp_xml);  
                 input_cascade.fish_per_level=atoi(temp_string);
                   

               }
              //xmlFree(level_xml);
           }



          
         }


}


//parse lasergame
void parse_lasergame(xmlNode *cur_node)
{
 xmlChar *temp_xml;
 xmlNode *child_node;
  char temp_string[5];

       //printf("Element: %s \n", cur_node->name); 


        for(child_node = cur_node->children; child_node != NULL; child_node = child_node->next)
        {
           if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"words_file") )
           {
             
            
              temp_xml= xmlNodeGetContent(child_node);
              if(temp_xml)
               {

                 sprintf(input_laser.filepath ,"%s", temp_xml); 

               }
              //xmlFree(filename);
           }          
          else if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"level") )
           {
              
               //the level value is got from XML file
               temp_xml= xmlNodeGetContent(child_node);
              if(temp_xml)
               {
                 sprintf(temp_string, "%s", temp_xml); 
                 input_laser.level=atoi(temp_string);
                   

               }
              //xmlFree(level_xml);
           }         
  //       else if ( cur_node->type == XML_ELEMENT_NODE  &&
  //             !xmlStrcmp(child_node->name, (const xmlChar *)"lives") )
  //           {
              
               //the level value is got from XML file
  //                temp_xml= xmlNodeGetContent(child_node);
  //            if(temp_xml)
  //             {
  //               sprintf(temp_string, "%s", temp_xml);  
  //               input_laser.num_of_lives=atoi(temp_string);
                   

  //              }
              //xmlFree(level_xml);
 //          }           

        }
}


//parse phrases
void parse_phrases(xmlNode *cur_node)
{
 xmlChar *filename;
 xmlNode *child_node;


       //printf("Element: %s \n", cur_node->name); 

        // For each child of factors: i.e. wave
        for(child_node = cur_node->children; child_node != NULL; child_node = child_node->next)
        {
           if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"phrases_file") )
           {             
            
              filename= xmlNodeGetContent(child_node);
              if(filename)
               {
                 //printf("         Wave: %s\n", wave);
                 sprintf(input_phrases.phrases_path ,"%s", filename); 

               }
              //xmlFree(filename);
           }          
         
         }


}



//parse typing lesson
void parse_typing_lesson(xmlNode *cur_node)
{
 xmlChar *filename;
 xmlNode *child_node;


       //printf("Element: %s \n", cur_node->name); 

        // For each child of factors: i.e. wave
        for(child_node = cur_node->children; child_node != NULL; child_node = child_node->next)
        {
           if ( cur_node->type == XML_ELEMENT_NODE  &&
                !xmlStrcmp(child_node->name, (const xmlChar *)"filepath") )
           {             
            
              filename= xmlNodeGetContent(child_node);
              if(filename)
               {
                 //printf("         Wave: %s\n", wave);
                 sprintf(input_typing_lesson.filepath ,"%s", filename); 

               }
              //xmlFree(filename);
           }          
         
         }


}



//write cascade
void write_cascade()
{
//int k;
xmlNodePtr node = NULL;/* node pointer */

node = xmlNewChild(root_write, NULL, BAD_CAST "cascade", NULL);

          
            sprintf(buff, "%d", result_cascade.level_reached);
            xmlNewChild(node, NULL, BAD_CAST "level_reached", BAD_CAST buff); 


            sprintf(buff, "%d", result_cascade.fish_eaten);
            xmlNewChild(node, NULL, BAD_CAST "fish_eaten", BAD_CAST buff); 


}

//write phrases
void write_phrases(int choice)
{
//int k;
xmlNodePtr node = NULL;/* node pointer */
if(choice==0)
node = xmlNewChild(root_write, NULL, BAD_CAST "phrases", NULL);
else if (choice==1)
node = xmlNewChild(root_write, NULL, BAD_CAST "typing_lesson", NULL);
else 
return;
          
            sprintf(buff, "%.2f", result_phrases.time);
            xmlNewChild(node, NULL, BAD_CAST "time", BAD_CAST buff); 


            sprintf(buff, "%d", result_phrases.correct_chars);
            xmlNewChild(node, NULL, BAD_CAST "correct_chars", BAD_CAST buff); 


            sprintf(buff, "%d", result_phrases.errors);
            xmlNewChild(node, NULL, BAD_CAST "errors", BAD_CAST buff); 

}



//write laser
void write_laser()
{
//int k;
xmlNodePtr node = NULL;/* node pointer */

node = xmlNewChild(root_write, NULL, BAD_CAST "laser", NULL);

          
            sprintf(buff, "%d", result_laser.wave);
            xmlNewChild(node, NULL, BAD_CAST "wave_reached", BAD_CAST buff); 


            sprintf(buff, "%d", result_laser.score);
            xmlNewChild(node, NULL, BAD_CAST "score", BAD_CAST buff); 


}



void clean_up()
{


  

  /* Free all images and sounds used by SDL: */
  Cleanup_SDL_Text();

SDL_FreeSurface(screen);
  screen = NULL;
  
//free(input);
//free(result);
 free(game_completed);

  SDL_Quit();
/*free the document read*/
  xmlFreeDoc(doc_read);
    /*free the document write*/
    xmlFreeDoc(doc_write);
  /*
   *Free the global variables that may
   *have been allocated by the parser.
   */
  xmlCleanupParser();
//free(xml_lesson_path);


}



