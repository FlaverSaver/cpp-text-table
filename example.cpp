#include <iostream>
#include "TextTable.h"

main()
{
    TextTable t( '-', '|', '+' );

    t.add( "" );
    t.add( "Sex", TextTable::Alignment::CENTER);
    t.add( "Age", TextTable::Alignment::CENTER );
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
    t.add( "female" );
    t.endOfRow();

    t.add( "Bob" );
    t.add( "male" );
    t.add( "25" );
    t.endOfRow();

    t.setMinWidth(0, 10);

    t.setMinWidth(1, 8);

    t.setAlignment( 2, TextTable::Alignment::RIGHT );
    t.setMinWidth(2, 8);

    std::cout << t;
    return 0;
}
