#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>



class CData
{

//ds attributes
private:

    //ds city struct
    struct CCity
    {
        //constructor
        CCity( std::string& p_strName, unsigned int& p_uCityCode, unsigned int& p_uCountryCode ):strName( p_strName ),
                                                                                                 uCityCode( p_uCityCode ),
                                                                                                 uCountryCode( p_uCountryCode )
        {
            //ds nothing to do
        }

        //ds basic properties
        std::string strName;
        unsigned int uCityCode;
        unsigned int uCountryCode;

        //ds vector with possible names
        std::vector< std::string > vecNames;
    };

    //ds vector of city structs
    std::vector< CCity > m_vecCities;

    //ds vector containing countries with their cities
    std::vector< std::vector< CCity > > m_vecCountries;

//ds accessors
public:

    //ds imports data and adds it to the cities
    void importData( const std::string& p_strFilepath )
    {
        //ds get a reading handle
        std::ifstream cReader( p_strFilepath.c_str( ), std::ifstream::in );

        //ds if we can read
        if( 0 != cReader )
        {
            //ds while the file handle is good
            while( cReader.good( ) )
            {
                //ds reading buffer
                char chBuffer[256];

                //ds get current line
                cReader.getline( chBuffer, 256 );

                //ds get buffer into a string for parsing operations
                std::string strCurrentLine( chBuffer );

                //ds break if empty
                if( strCurrentLine.empty( ) ){ break; }

                //ds parse city name, city code and country code - get the 2 data separators
                const std::size_t uIndexSeparator1 = strCurrentLine.find_first_of( ',' );
                const std::size_t uIndexSeparator2 = strCurrentLine.find_last_of( ',' );

                //ds extract all properties
                std::string strCityName    = strCurrentLine.substr( 0, uIndexSeparator1 );
                unsigned int uCityCode     = std::stoi( strCurrentLine.substr( uIndexSeparator1+1, ( uIndexSeparator2-uIndexSeparator1 - 1 ) ) );
                unsigned int uCountryCode  = std::stoi( strCurrentLine.substr( uIndexSeparator2+1, ( strCurrentLine.length( ) - uIndexSeparator2 - 1 ) ) );

                //ds transform city name to lower case letters only
                std::transform( strCityName.begin( ), strCityName.end( ), strCityName.begin( ), ::tolower );

                //ds add the new element
                m_vecCities.push_back( CCity( strCityName, uCityCode, uCountryCode ) );
            }
        }

        //ds import done
        std::cout << "[ <CData>(importData)      ] data import finished for: " << p_strFilepath << " added: " << m_vecCities.size( ) << " new cities" << std::endl;
    }

    //ds create country vectors
    void createCountries( )
    {
        //ds sort vector by country codes
        std::sort( m_vecCities.begin( ), m_vecCities.end( ), _IsSmallerCountryCode );

        //ds start adding the countries to the list with all their cities - we need a counter
        unsigned int uCurrentCountryCode = m_vecCities.front( ).uCountryCode;

        //ds push back the current country and an empty vector
        m_vecCountries.push_back( std::vector< CCity >( ) );

        //ds counter for number of countries
        unsigned int uCurrentIndexOfCountries = 0;

        //ds loop over the sorted vector
        for( unsigned int u = 0; u < m_vecCities.size( ); ++u )
        {
            //ds if the country code matches
            if( uCurrentCountryCode == m_vecCities[u].uCountryCode )
            {
                //ds add it to the current country vector
                m_vecCountries[uCurrentIndexOfCountries].push_back( m_vecCities[u] );
            }
            else
            {
                //ds new code
                uCurrentCountryCode = m_vecCities[u].uCountryCode;

                //ds increment country counter
                ++uCurrentIndexOfCountries;

                //ds add the new country vector
                m_vecCountries.push_back( std::vector< CCity >( ) );

                //ds add the element
                m_vecCountries[uCurrentIndexOfCountries].push_back( m_vecCities[u] );
            }
        }

        std::cout << "[ <CData>(createCountries) ] country creation complete added: " << m_vecCountries.size( ) << " countries" << std::endl;
    }

    //ds merge cities - this is the trick
    void mergeCities( )
    {
        //ds for all countries
        for( unsigned int u = 0; u < m_vecCountries.size( ); ++u )
        {
            //ds sort the city vector by city code
            std::sort( m_vecCountries[u].begin( ), m_vecCountries[u].end( ), _IsSmallerCityCode );

            //ds current treated cities with multiplicatives to be merged
            std::vector< std::vector< CCity > >vecCurrentCities;

            //ds start adding the cities with their multiplicatives to the list
            unsigned int uCurrentCityCode = m_vecCountries[u].front( ).uCityCode;

            //ds add a new empty vector for the current
            vecCurrentCities.push_back( std::vector< CCity >( ) );

            //ds counter for number of cities
            unsigned int uCurrentIndexOfCities = 0;

            //ds add all cities with same index
            for( unsigned int v = 0; v < m_vecCountries[u].size( ); v++ )
            {
                //ds if index matches
                if( uCurrentCityCode == m_vecCountries[u][v].uCityCode )
                {
                    //ds add the city
                    vecCurrentCities[uCurrentIndexOfCities].push_back( m_vecCountries[u][v] );
                }
                else
                {
                    //ds new code
                    uCurrentCityCode = m_vecCountries[u][v].uCityCode;

                    //ds increment country counter
                    ++uCurrentIndexOfCities;

                    //ds add the new country vector
                    vecCurrentCities.push_back( std::vector< CCity >( ) );

                    //ds add the element
                    vecCurrentCities[uCurrentIndexOfCities].push_back( m_vecCountries[u][v] );
                }
            }

            std::cout << "[ <CData>(mergeCities)     ] currently merging country code: " << m_vecCountries[u].front( ).uCountryCode << " cities: " << vecCurrentCities.size( ) << std::endl;

            //ds informative counter
            unsigned int uTotalNumberOfCities           = 0;
            unsigned int uTotalNumberOfCityNamesRemoved = 0;

            //ds for each city vector
            for( unsigned int v = 0; v < vecCurrentCities.size( ); ++v )
            {
                //ds string vector with names to minimize - another vector to improve readability
                std::vector< std::string > vecNames;

                //ds fill in all names
                for( unsigned int w = 0; w < vecCurrentCities[v].size( ); ++w )
                {
                    //ds add the current name
                    vecNames.push_back( vecCurrentCities[v][w].strName );
                }

                //ds update counter
                uTotalNumberOfCities += vecNames.size( );

                //ds loop through the string list
                for( std::vector< std::string >::iterator itName = vecNames.begin( ); itName != vecNames.end( ); ++itName )
                {
                    //ds get current name
                    const std::string strCurrentName = *itName;

                    //ds check if there is a double entry
                    const std::vector< std::string >::iterator itNameBefore = std::find( vecNames.begin( ), itName-1, strCurrentName );
                    const std::vector< std::string >::iterator itNameAfter  = std::find( itName+1, vecNames.end( ), strCurrentName );

                    //ds if we got a double entry before the current one
                    if( itName-1 != itNameBefore && "" != *itNameBefore )
                    {
                        //ds empty the string
                        *itNameBefore = "";
                        ++uTotalNumberOfCityNamesRemoved;
                    }

                    //ds if we got a double entry after the current one
                    if( vecNames.end( ) != itNameAfter && "" != *itNameAfter )
                    {
                        //ds empty the string
                        *itNameAfter = "";
                        ++uTotalNumberOfCityNamesRemoved;
                    }
                }
            }

            std::cout << "[ <CData>(mergeCities)     ] stage 1 (double names) clear - removed " << uTotalNumberOfCityNamesRemoved << "/" << uTotalNumberOfCities << " city names" << std::endl;

        }
    }

//ds boolean for sort
private:

    static bool _IsSmallerCountryCode( const CCity& p_cCity1, const CCity& p_cCity2 )
    {
        //ds return smaller country code
        return ( p_cCity1.uCountryCode < p_cCity2.uCountryCode );
    }

    static bool _IsSmallerCityCode( const CCity& p_cCity1, const CCity& p_cCity2 )
    {
        //ds return smaller country code
        return ( p_cCity1.uCityCode < p_cCity2.uCityCode );
    }

};


int main( int argc, char** argv )
{
    //ds get a data object
    CData cTraining;

    //ds import data
    cTraining.importData( "training.csv" );

    //ds merge data
    cTraining.createCountries( );
    cTraining.mergeCities( );


    return 0;
}
