//
//  ftdiAcqui.h
//  ftidAcqui
//
//  Created by Philippe Fouquet on 09/09/13.
//  Copyright (c) 2013 Philippe Fouquet. All rights reserved.
//

#ifndef ftidAcqui_ftdiAcqui_h
#define ftidAcqui_ftdiAcqui_h

int ftdiAcqui( char* file );
int ftdiConnect( void );
void ftdiClose( void );
double ftdiGetValue( int id, int voie );

#endif
