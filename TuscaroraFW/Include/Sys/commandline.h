////////////////////////////////////////////////////////////////////////////////// 
// Tuscarora Software Suite. The Samraksh Company.  All rights reserved. 
// Please see License.txt file in the software root directory for usage rights. 
// Please see developer's manual for implementation notes. 
////////////////////////////////////////////////////////////////////////////////// 

#ifndef _COMMANDLINE_H
#define _COMMANDLINE_H

#include <sstream> // stringstream
// #include <iostream> // cout // don't use because dce doesn't like cout. Use printf instead.
#include "Lib/DS/ListT.h"
#include <map> // map type
#include <string> // string type
#include <typeinfo> // typeid
#include <cstring> // strchr, strncmp
#include <cstdlib> // exit
#include <cstdio> // printf
#include <cerrno> // errno

// using std::cout;
using std::map;
using std::stringstream;
using std::string;

/// Simple command-line parser modeled after ns3 CommandLine.
///
/// Add values to initialize by calling .addValue( string name, T
/// &value ) where T is the type of value. This will configure the
/// parser set the variable "value" when it encounters "name".  For
/// example: ```opts.addValue( "--count", &count ); opts.parse(argc,
/// argv); ``` will set the variable count to 4 when parsing
/// "--count=4" or "--count 4".
///
/// The methods "addValue" and "addChoice" return the parser, so they
/// can be strung together. See the example program below.
/// 
/// The "parse ( int argc, char *argv[] )" method returns true if it
/// parses with no errors. It is an error to encounter an unknown
/// option.
/// 
/// Parsing storts from argv[0], so if you're calling parse directly
/// from main, you'll probably want to skip the first argv (which
/// contains the program name.)  Eg: args.parse(argc-1, argv+1)
/// 
/// Parsing stops when a double dash (--) is encountered and the argc
/// and argv members are set where parsing left off. But, note that
/// when *printing* the args with str(), all the args are given, even
/// the ones that were not parsed. For example:
///
///            "prog arg1=v1 arg2=v2 -- arg3 arg4"
///
/// "Opts opts(argc-1, argv+1);" will parse arg1 and arg2 but stop at the
/// -- and not parse arg3 and arg4. But if the args are printed out
/// with 'printf("%s", args.str().c_str());', then the result will be
///
///            args1=v1 args2=v2 args3 args4.

#ifdef _example_use_ // this is a documenting example
#include "commandline.h"

using std::stringstream;
//using std::cout;

int main ( int argc, char** argv ) {

  int x=1, y=2;  // declare two ints with default values
  float pi=3.14; // declare float with default value
  c++  string str="default";    // declare string with default value

  Opts opts; // declare an Opts instance and configure it.
  opts
    .addValue("x", "Help string for x value", &x) // add option for int x
    .addValue("y", "Help string for y value", &y) // add option for int y
    .addChoice( 1 )  // add choices for y (the last option)
    .addChoice( 2 )
    .addChoice( 3 )
    .addValue("pi", "Help string for pi value", &pi)
    .addValue("string", "Help string for string value", &str);

  stringstream ss;
  ss << "Usage:\n" << argv[0] << " [options]\n"
     << opts.usage() << "\n\n";

  ss << "Parsing arguments to " << argv[0] << "\n\n";
  // Note that parsing starts from argv[0], so call parse on arc-1 and argv+1.
  if ( ! opts.parse ( argc-1, argv+1 ) ) {
    ss << "error parsing\n";
    ss << opts.usage();
  }

  ss << "\nDumping values before exit:\n" << opts.dump();
  printf ( "%s", ss.str().c_str() );

};
#endif //  example

inline string typestr ( bool v ) { return "bool"; };
inline string typestr ( int v ) { return "int"; };
inline string typestr ( unsigned v ) { return "unsigned"; };
inline string typestr ( unsigned long v ) { return "unsigned long"; };
inline string typestr ( long v ) { return "long"; };
inline string typestr ( double v ) { return "double"; };
inline string typestr ( float v ) { return "float"; };
inline string typestr ( string v ) { return "string"; };
inline string typestr ( char* v ) { return "char*"; };
inline string typestr ( char** v ) { return "char**"; };

class Opts {

 public:
  // argc and argv are set to point to the parameters after the "--"
  int argc;
  char **argv;

  Opts() {
    this->argc = -1;
    this->argv = (char **) 0;
  };

  template < class T >
    Opts &addValue ( string name, string help, T* v ) {
    stringstream ss;

    // check for duplicates
    for ( unsigned i = 0; i < opts.Size(); i++) {
      if ( name == opts[i]->name ) {
	ss << "Error: duplicate " << name << "\n";
	printf ( "%s", ss.str().c_str() );
	exit ( 1 );
      }
    }
    Opt *o = (Opt*)new OptBasic < T > ( name, help, v );
    opts.InsertBack( o );
    return *this;
  };

  template < class T >
    Opts &addChoice ( T c ) {
    stringstream ss;

    int last = opts.Size() - 1;
    if ( last <  0 ) {
      ss << "Error: no option to add a choice to.";
      printf ( "%s", ss.str().c_str() );
      exit (1);
    }

    OptBasic < T > *opt =
      dynamic_cast < OptBasic<T>* > ( opts[ last ] );
    if ( ! opt ) {
      // type mismatch;
      ss << "Error: can't add choice because of a type mismatch.\n";
      ss << "Choice " << c << " has type " << typestr ( c )
	 << " but option " << opts[last]->name
	 << " has type " << opts[last]->type() << "\n\n";
      printf ( "%s", ss.str().c_str() );
      exit ( 1 );
    }
    opt->addChoice ( c );
    return *this;
  };


  bool parse ( int argc, char *argv[] ) {
    stringstream ss;

    bool ok = true;
		for ( int i = 0 ; i < argc ; i++ ) {
			if ( ! strcmp ( "--", argv[i] )) {
				// stop parsing, set the argc and argv members.
				this->argc = argc-i-1;
				this->argv = argv+i+1;
				return ok;
			}
			char* value = strchr ( argv[i], '=' );
			string name = value? string(argv[i], value): argv[i];
			if (value) value += 1; // move value off the '='

			ok = false;

			for ( unsigned j = 0; j < opts.Size(); j++ ) {
				Opt *o = opts[j];
				// printf ( "commandline parse: name = %s\n", o->name.c_str() );

				if ( o->name == name ) {
					if ( value ) {
						ok = o->parse ( value );
						if ( !ok ) {
							ss << "Error parsing value (" << value << ") "
							<< "for option " << name << "\n";
							printf ( "%s", ss.str().c_str() );
							return false;
						}
					else break; // correctly parsed this option, go to the next one.
					}
					else if ( o->isflag ()) {
						ok = o->parse ((char*) "true" );
						if ( !ok ) {
							ss << "Error parsing bool value (true) for option " << name
							<< "\n";
							printf ( "%s", ss.str().c_str() );
							return false;
						}
						else break; // correctly parsed this option, go to the next one.
					}
					else if ( i + 1 < argc ) {
						ok = o->parse ( argv[++i] );
						if ( !ok ) {
							ss << "Error parsing value (" << argv[i] << ") "
							<< "for option " << name << "\n";
							printf ( "%s", ss.str().c_str() );
							return false;
						}
					else break; // correctly parsed this option, go to the next one.
					}
					else {
						ss << "Missing value for argument " << i << "(" << argv[i] << ")\n";
						printf ( "%s", ss.str().c_str() );
						return false;
					}
				}
			}//End for
			
			if ( ! ok ) {
				ss << "Error parsing unknown option " << name << "\n";
				printf ( "%s", ss.str().c_str() );
				return false; // not found
			}
		}
		return ok;
	};

  string usage () {
    stringstream s;
    for ( unsigned i = 0; i < opts.Size(); i++ ) {
      s << opts[i]->usage() << "\n";
    }
    return s.str();
  };

  string dump () {
    stringstream s;
    for ( unsigned i = 0; i < opts.Size(); i++ ) {
      s << opts[i]->dump() << "\n";
    }
    if ( this->argc > 0 ) {
      s << "argc = " << this->argc << ", argv =\n";
      for ( int i = 0; i < this->argc; i++ ) {
	s << "  [" << i << "] " << this->argv[i] << "\n";
      }
    }
    return s.str();
  };

  string str () {
    stringstream s;

    for ( unsigned i = 0; i < opts.Size(); i++ ) {
      if ( i ) s << " ";
      // note that we are not escaping anything
      s << opts[i]->name << "=" << opts[i]->str();
    }
    if ( this->argc > 0 ) {
      s << " ";
      for ( int i = 0; i < this->argc; i++ ) {
	if ( i ) s << " ";
	s << this->argv[i];
      }
    }
    return s.str();
  };

  struct Opt {
    // http://stackoverflow.com/questions/7405740/how-can-i-initialize-base-class-member-variables-in-derived-class-constructor
    //  protected:
  public:
    Opt ( string n, string h ): name(n), help(h), hasChoices(false) {};

    string name;
    string help;
    bool hasChoices;

    virtual bool isflag() = 0; // boolean flags are handled
    // differently; they don't have values
    virtual bool parse ( char* ) = 0;
    virtual string usage() = 0;
    virtual string str() = 0;
    virtual string dump() = 0;
    virtual string type() = 0;
    virtual string parseMsg ( bool ok, char* v ) {
      return ""; // skip diagnostics if nothing is wrong.
      stringstream s;
      s << ( ok ? "successfully ": "unsuccessfully " ) << "parsed "
	<< "(" << this->type() << ") "
	<< this->name << " " << this->str()
	<< " (from char* \"" << v <<"\")\n";
      if ( ! ok ) {
	s << "Error: errno " << errno << ": " << strerror ( errno ) << "\n";
	printf ( "%s\n", s.str().c_str() ); // cout << s.str();
      }
      // printf ( "%s\n", s.str().c_str() ); // cout << s.str();
      return s.str();
    };
  };

  template < class T >
    struct OptBasic: public Opt {

    // public:
    T *value;
    T defaultValue;
    ListT < T, false, EqualTo<T> > choices;
    OptBasic ( string n, string h, T* v ):
    Opt(n, h), value(v), defaultValue(*v) {};

    template < class V > bool addChoice ( V x ) {
      this->choices.InsertBack( x );
      this->hasChoices = true;
      return true;
    };

    virtual string usage() {
      stringstream s;
      s << this->name << "\n";

      // help
      unsigned h0=0, h1;
      while ( h0 < this->help.size() ) {
	h1 = this->help.find ( '\n', h0 );
	s << "  " << this->help.substr ( h0, h1-h0 ) << "\n";
	h0 = (h1 < h1+1)? h1+1: this->help.size();
      }

      s << "  default value (" << this->type() << ") " << defaultValue << "\n";

      if ( this->hasChoices ) {
	unsigned nchoices = this->choices.Size();
	if ( nchoices == 0 ) s << "No valid values.\n";
	else {
	  s << "  Choices are: \n";
	  for ( unsigned i = 0; i < nchoices; i++ ) {
	    s << "    " << this->choices[i] << "\n";
	  }
	}
      }
      return s.str();
    };

    virtual string str() {
      stringstream s;
      s  << *(this->value);
      return s.str();
    };

    virtual string dump() {
      stringstream s;
      s << this->name << "=" << *(this->value);
      return s.str();
    };

    virtual string type() {
      return typestr ( this->defaultValue );
    };

    virtual bool checkChoices() {
      stringstream ss;

      if ( ! this->hasChoices ) return true;
      for ( unsigned i = 0; i < this->choices.Size(); i++ ) {
	if ( this->choices[i] == *(this->value) ) return true;
      }
      ss << "Bad value (" << *(this->value) << ") "
	 << "for option " << this->name << ". "
	 << "Must be one of:\n";
      for ( unsigned i = 0; i < this->choices.Size(); i++ ) {
	ss << "  " << this->choices[i] << "\n";
      }
      printf ( "%s", ss.str().c_str() );
      return false;
    };

    virtual bool parse( char* v ) {
      printf ( "default parser returns false\n" );
      printf ( "Returning false for type %s.\n", this->type().c_str() );
      return false;
    };

    bool isflag () { return false; };
  }; // end declaration of OptBasic

  ListT < Opt*, false, EqualTo<Opt*> > opts;

}; // end declaration of Opts

// inline to prevent compiler and linker error

template <> inline bool Opts::OptBasic < bool >::
isflag () { return true; };

template <> inline bool Opts::OptBasic < bool >::
parse ( char* v ) { 
  stringstream ss;

  bool ok = false;
  if ( strncmp ((char*) "true", v, strlen ( v )) == 0 ||
       strncmp ((char*) "TRUE", v, strlen ( v )) == 0 ||
       strncmp ((char*) "1", v, strlen ( v )) == 0 ) {
    *(this->value) = true;
    ok = true;
  }
  else if ( strncmp ((char*) "false", v, strlen ( v )) == 0 ||
	    strncmp ((char*) "FALSE", v, strlen ( v )) == 0 ||
	    strncmp ((char*) "0", v, strlen ( v )) == 0 ) {
    *(this->value) = false;
    ok = true;
  }
  else {
    ss << "Not a bool value: " << v << "\n";
    printf ( "%s", ss.str().c_str() );
  }
  ok = ok && this->checkChoices();
  this->parseMsg ( ok, v );
  return ok;
};

template <> inline bool Opts::OptBasic < unsigned >::
parse ( char* v ) {
  int i = atoi(v);
  // printf ("read %d\n", i);
  if ( i < 0 ) {
    printf ("unsigned value should not be negative.\n");
    exit (1);
  }
  *(this->value) = i;
  return true;
};

template <> inline bool Opts::OptBasic < int >::
parse ( char* v ) {
  int i = atoi(v);
  // printf ("read %d\n", i);
  *(this->value) = i;
  return true;
};

template <> inline bool Opts::OptBasic < double >::
parse ( char* v ) {
  double i = atof(v);
  // printf ("read %f\n", i);
  *(this->value) = i;
  return true;
};

template <> inline bool Opts::OptBasic < string >::
parse ( char* v ) {
  *(this->value) = v;
  return true;
};

#endif // _COMMANDLINE_H
