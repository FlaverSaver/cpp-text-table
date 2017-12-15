#include <iostream>
#include "TextTable.h"

main()
{
    TextTable t( '-', '|', '+' );

    t.add( "" );
    t.add( "Sex" );
    t.add( "Age" );
    t.endOfRow();

    t.add( "Moses" );
    t.add( "male" );
    t.add( "4556" );
    t.endOfRow();

    t.add( "Jesus" );
    t.add( "male" );
    t.add( "2016" );
    t.endOfRow();

    t.add( "Debora" );
    t.add( "female" );
    t.add( "3001" );
    t.endOfRow();

    t.add( "Bob" );
    t.add( "male" );
    t.add( "25" );
    t.endOfRow();

    t.setAlignment(0, TextTable::Alignment::CENTER);
    t.setMinWidth(0, 10);

    t.setMinWidth(1, 8);

    t.setAlignment( 2, TextTable::Alignment::RIGHT );
    t.setMinWidth(2, 8);

    std::cout << t;
    return 0;
}
