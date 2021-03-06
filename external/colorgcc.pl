#! /usr/bin/perl -w

#
# colorgcc
#
# Version: 2.0.5
#
# $Id: colorgcc,v 1.10 1999/04/29 17:15:52 jamoyers Exp $
#
# A wrapper to colorize the output from compilers whose messages
# match the "gcc" format.
#
# Requires the ANSIColor module from CPAN.
#
# Usage:
#
# In a directory that occurs in your PATH _before_ the directory
# where the compiler lives, create a softlink to colorgcc for
# each compiler you want to colorize:
#
#    g++ -> colorgcc
#    gcc -> colorgcc
#    cc  -> colorgcc
#    etc.
#
# That's it. When "g++" is invoked, colorgcc is run instead.
# colorgcc looks at the program name to figure out which compiler to run.
#
# The default settings can be overridden with ~/.colorgccrc.
# See the comments in the sample .colorgccrc for more information.
#
# Note:
#
# colorgcc will only emit color codes if:
#
#    (1) Its STDOUT is a tty and
#    (2) the value of $TERM is not listed in the "nocolor" option.
#
# If colorgcc colorizes the output, the compiler's STDERR will be
# combined with STDOUT. Otherwise, colorgcc just passes the output from
# the compiler through without modification.
#
# Author: Jamie Moyers <jmoyers@geeks.com>
# Started: April 20, 1999
# Licence: GNU Public License
#
# Credits:
#
#    I got the idea for this from a script called "color_cvs":
#       color_cvs .03   Adrian Likins <adrian@gimp.org> <adrian@redhat.com>
#
#    <seh4@ix.netcom.com> (Scott Harrington)
#       Much improved handling of compiler command line arguments.
#       exec compiler when not colorizing to preserve STDOUT, STDERR.
#       Fixed my STDIN kludge.
#
#    <ecarotti@athena.polito.it> (Elias S. G. Carotti)
#       Corrected handling of text like -DPACKAGE=\"Package\"
#       Spotted return code bug.
#
#    <erwin@erwin.andreasen.org> (Erwin S. Andreasen)
#    <schurchi@ucsd.edu> (Steve Churchill)
#       Return code bug fixes.
#
#    <rik@kde.org> (Rik Hemsley)
#       Found STDIN bug.
#
# Changes:
#
# 1.3.2 Better handling of command line arguments to compiler.
#
#       If we aren't colorizing output, we just exec the compiler which
#       preserves the original STDOUT and STDERR.
#
#       Removed STDIN kludge. STDIN being passed correctly now.
#
# 1.3.1 Added kludge to copy STDIN to the compiler's STDIN.
#
# 1.3.0 Now correctly returns (I hope) the return code of the compiler
#       process as its own.
#
# 1.2.1 Applied patch to handle text similar to -DPACKAGE=\"Package\".
#
# 1.2.0 Added tty check. If STDOUT is not a tty, don't do color.
#
# 1.1.0 Added the "nocolor" option to turn off the color if the terminal type
#       ($TERM) is listed.
#
# 1.0.0 Initial Version

use strict;

use Term::ANSIColor;
use IPC::Open3;
use Cwd 'abs_path';

my(%nocolor, %colors, %compilerPaths);
my($unfinishedQuote, $previousColor);

sub initDefaults
{
   $nocolor{"dumb"} = "true";

   $colors{"srcColor"} = color("cyan");
   $colors{"introFileNameColor"} = color("blue");
   $colors{"introMessageColor"} = color("blue");

   $colors{"warningFileNameColor"} = color("yellow");
   $colors{"warningNumberColor"}   = color("yellow");
   $colors{"warningMessageColor"}  = color("yellow");

   $colors{"errorFileNameColor"} = color("bold red");
   $colors{"errorNumberColor"}   = color("bold red");
   $colors{"errorMessageColor"}  = color("bold red");

   $colors{"noteFileNameColor"} = color("green");
   $colors{"noteNumberColor"}   = color("green");
   $colors{"noteMessageColor"}  = color("green");
}

sub loadPreferences
{
# Usage: loadPreferences("filename");

   my($filename) = @_;

   open(PREFS, "<$filename") || return;

   while(<PREFS>)
   {
      next if (m/^\#.*/);          # It's a comment.
      next if (!m/(.*):\s*(.*)/);  # It's not of the form "foo: bar".

      my $option = $1;
      my $value = $2;

      if ($option =~ m/Color/)
      {
          $colors{$option} = color($value);
      }
      elsif ($option eq "nocolor")
      {
         # The nocolor option lists terminal types, separated by
         # spaces, not to do color on.
         foreach my $term (split(' ', $value))
         {
            $nocolor{$term} = 1;
         }
      }
      else
      {
         $compilerPaths{$option} = $value;
      }
   }
   close(PREFS);
}

sub srcscan
{
# Usage: srcscan($text, $normalColor)
#    $text -- the text to colorize
#    $normalColor -- The escape sequence to use for non-source text.

# Looks for text between ` and ', and colors it srcColor.

   my($line, $normalColor) = @_;

   if (defined $normalColor)
   {
      $previousColor = $normalColor;
   }
   else
   {
      $normalColor = $previousColor;
   }

   my($srcon) = color("reset") . $colors{"srcColor"};
   my($srcoff) = color("reset") . $normalColor;

   $line = ($unfinishedQuote? $srcon : $normalColor) . $line;

   # These substitutions replaces `foo' with `AfooB' where A is the escape
   # sequence that turns on the the desired source color, and B is the
   # escape sequence that returns to $normalColor.

   # Handle multi-line quotes.
   if ($unfinishedQuote)
   {
      if ($line =~ s/^([^\`]*?)\'/$1$srcoff\'/)
      {
         $unfinishedQuote = 0;
      }
   }
   if ($line =~ s/\`([^\']*?)$/\`$srcon$1/)
   {
      $unfinishedQuote = 1;
   }

   # Single line quoting.
   $line =~ s/[\`\'](.*?)\'/\`$srcon$1$srcoff\'/g;

   print($line, color("reset"));
}

#
# Main program
#

# Set up default values for colors and compilers.
initDefaults();

# Read the configuration file, if there is one.
my $configFile = "~/.colorgccrc";
if (-f $configFile)
{
   loadPreferences($configFile);
}
elsif (-f '/etc/colorgcc/colorgccrc')
{
   loadPreferences('/etc/colorgcc/colorgccrc');
}

# Set our default output color.  This presumes that any unrecognized output
# is an error.
$previousColor = $colors{"errorMessageColor"};

# Figure out which compiler to invoke based on our program name.
$0 =~ m%.*/(.*)$%;
my $progName = $1 || $0;
my $compiler_pid;

# If called as "colorgcc", just filter STDIN to STDOUT.
if ($progName =~ 'colorgcc')
{
   open(GCCOUT, "<&STDIN");
}
else
{
   # See if the user asked for a specific compiler.
   my $compiler;
   if (!defined($compiler = $compilerPaths{$progName}))
   {
      # Find our wrapper dir on the PATH and tweak the PATH to remove
      # everything up-to and including our wrapper dir.
      if ($0 =~ m#(.*)/#)
      {
         # We were called with an explicit path, so trim that off the PATH.
         my $find = $1;
         $find = abs_path($1) unless $find =~ m#^/#;
         $ENV{'PATH'} =~ s#.*(^|:)\Q$find\E(:|$)##;
      }
      else
      {
         my(@dirs) = split(/:/, $ENV{'PATH'});
         while (defined($_ = shift @dirs))
         {
            if (-x "$_/$progName")
            {
               $ENV{'PATH'} = join(':', @dirs);
               last;
            }
         }
      }
      $compiler = $progName;
   }

   # Get the terminal type.
   my $terminal = $ENV{"TERM"} || "dumb";

   # If it's in the list of terminal types not to color, or if
   # we're writing to something that's not a tty, don't do color.
   if (! -t STDOUT || $nocolor{$terminal})
   {
      exec $compiler, @ARGV
         or die("Couldn't exec");
   }

   # Keep the pid of the compiler process so we can get its return
   # code and use that as our return code.
   $compiler_pid = open3('<&STDIN', \*GCCOUT, \*GCCOUT, $compiler, @ARGV);
}

# Colorize the output from the compiler.
if ($progName eq "gcc" || $progName eq "g++" || $progName =~ 'colorgcc')
{
   while(<GCCOUT>)
   {
      if (m#^(.+?\.[^:/ ]+:)(\d+:)(.*)$#) # filename:lineno:message
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";

         if ($field3 =~ m/\s+warning:.*/)
         {
            print($colors{"warningFileNameColor"}, "$field1", color("reset"));
            print($colors{"warningNumberColor"}, "$field2", color("reset"));
            srcscan("$field3\n", $colors{"warningMessageColor"});
         }
         # main.cpp:145:2: #error print() is not implemented for this system
         # gcc produces such "undefined reference" messages when debugging info
         # is available.
         # util.h:92: undefined reference to `util::read(int*, char const*);
         elsif ($field3 =~ m/\s+error:.*/ || $field3 =~ m/\d+:.*?#error.*/ ||
            $field3 =~ m/undefined reference/)
         {
            print($colors{"errorFileNameColor"}, "$field1", color("reset"));
            print($colors{"errorNumberColor"}, "$field2", color("reset"));
            srcscan("$field3\n", $colors{"errorMessageColor"});
         }
         else
         {
            print($colors{"noteFileNameColor"}, "$field1", color("reset"));
            print($colors{"noteNumberColor"}, "$field2", color("reset"));
            srcscan("$field3\n", $colors{"noteMessageColor"});
         }
      }
      # gcc produces such "undefined reference" messages w/o debugging info.
      # main.cpp:(.text+0x13): undefined reference to `floor(double)'
      elsif (m/^(.+?:)(\(.+?\):)( undefined reference to `.+?')$/)
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";
         print($colors{"errorFileNameColor"}, "$field1", color("reset"));
         print($colors{"errorNumberColor"}, "$field2", color("reset"));
         srcscan("$field3\n", $colors{"errorMessageColor"});
      }
      elsif (m/^(.*from)(.+?:)(\d+[,:])$/) # message:filename:line
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";
         # from /home/dgoncharov/src/main.cpp:63,
         print($colors{"noteMessageColor"}, "$field1", color("reset"));
         print($colors{"noteFileNameColor"}, "$field2", color("reset"));
         print($colors{"noteNumberColor"}, "$field3\n", color("reset"));
      }
      # node.cpp:(.text+0x3a86): warning: the use of `mktemp' is
      # dangerous, better use `mkstemp'
      elsif (m/^(.+?:)(\(.+?\):)( warning: .+?)$/)
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";
         print($colors{"warningFileNameColor"}, "$field1", color("reset"));
         print($colors{"warningNumberColor"}, "$field2", color("reset"));
         srcscan("$field3\n", $colors{"warningMessageColor"});
      }
      elsif (m/^:.+`.*'$/) # filename:message:
      {
         srcscan($_, $colors{"warningMessageColor"});
      }
      elsif (m/^(.*?:)(.+:)$/) # filename:message:
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         # No line number, treat as an "introductory" line of text.
         print($colors{"introFileNameColor"}, "$field1", color("reset"));
         srcscan("$field2\n", $colors{"introMessageColor"});
      }
      else # Anything else.
      {
         # Doesn't seem to be a warning or an error. Print normally.
         print(color("reset"), $_);
      }
   }
}
elsif ($progName eq "CC" || $progName =~ "^sun[cC]" || $^O eq "solaris")
{
   # Colorize the output from the compiler.
   while(<GCCOUT>)
   {
      # "main.cpp", line 8: Warning: Implicit int is not supported in C++.
      # "main.cpp", line 9: Error: The function "bar" must have a prototype.
      # "main.cpp":8: Warning: Implicit int is not supported in C++.
      # "main.cpp":9: Error: The function "bar" must have a prototype.
      # CC: Warning: "-g0" overrides "-g", debugging information will be
      # produced. Note, inlining will not be disabled.
      # "m.cpp", line 92: Warning (Anachronism): Formal argument 2 of type
      if (m/^(\".+?\",)( line [0-9]+:)(.+)$/) # filename:lineno:message
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";

         if ($field3 =~ m/Warning/)
         {
            print($colors{"warningFileNameColor"}, "$field1", color("reset"));
            print($colors{"warningNumberColor"}, "$field2", color("reset"));
            print($colors{"warningMessageColor"}, "$field3\n", color("reset"));
         }
         # "sstream.h", line 345: Warning, hidef: std::basic_stringstream
         # <char, std::char_traits<char>, std::allocator<char>>::rdbuf hides
         # the function std::ios::rdbuf(std::streambuf *).
         # "main.cpp", line 740:     Where, temwhilespec: While
         # specializing "std::basic_stringstream<char, std::char_traits<char>,
         # std::allocator<char>>".
         elsif ($field3 =~ m/Where,/)
         {
            print($colors{"noteFileNameColor"}, "$field1", color("reset"));
            print($colors{"noteNumberColor"}, "$field2", color("reset"));
            print($colors{"noteMessageColor"}, "$field3\n", color("reset"));
         }
         else
         {
            print($colors{"errorFileNameColor"}, "$field1", color("reset"));
            print($colors{"errorNumberColor"}, "$field2", color("reset"));
            print($colors{"errorMessageColor"}, "$field3\n", color("reset"));
         }
      }
      # ld: fatal: file test.mk: unknown file type
      # ld: fatal: File processing errors. No output written to a.out
      elsif (m/^ld: fatal: .+$/)
      {
         print($colors{"errorMessageColor"}, $_, color("reset"));
      }
      elsif (m/^CC: Warning: (.+)$/)
      {
         print($colors{"warningMessageColor"}, $_, color("reset"));
      }
      else # Anything else.
      {
         # Doesn't seem to be a warning or an error. Print normally.
         print(color("reset"), $_);
      }
   }
}
elsif ($progName =~ "^g?xl[cC].*" || $^O eq "aix")
{
   # Colorize the output from the compiler.
   while(<GCCOUT>)
   {
      # "test.cpp", line 9.12: 1540-0274 (S) The name lookup for "bar" did not
      # find a
      # declaration.
      # "test.cpp", line 15.1: 1540-1101 (W) A return value of type "int" is
      # expected.
      # "test.cpp", line 22.17: 1540-0256 (S) A parameter of type "const char
      # &" cannot
      # be initialized with an expression of type "const char [5]".
      # "test.cpp", line 22.17: 1540-1205 (I) The error occurred while
      # converting to
      # parameter 1 of "std::vector<char,std::allocator<char>
      # >::push_back(const char
      # &)".
      if (m/^(\".+?\",)( line [0-9\.]+:)( [-0-9]+.+)$/) # filename:lineno:message
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";

         if ($field3 =~ m/\((W|I)\)/)
         {
            print($colors{"warningFileNameColor"}, "$field1", color("reset"));
            print($colors{"warningNumberColor"}, "$field2", color("reset"));
            print($colors{"warningMessageColor"}, "$field3\n", color("reset"));
         }
         else
         {
            print($colors{"errorFileNameColor"}, "$field1", color("reset"));
            print($colors{"errorNumberColor"}, "$field2", color("reset"));
            print($colors{"errorMessageColor"}, "$field3\n", color("reset"));
         }
      }
      # /bb/util/version10-082009/usr/vacpp/bin/.orig/xlC_r: 1501-218 (W) file
      # test.mk contains an incorrect file suffix
      elsif (m/^.+?: [-0-9]+ \(W\).+$/)
      {
         print($colors{"warningMessageColor"}, $_, color("reset"));
      }
      # ld: 0711-715 ERROR: File test.mk cannot be processed.
      #  The file must be an object file, an import file, or an archive.
      elsif (m/^ld: [-0-9]+ ERROR: .+$/)
      {
          print($colors{"errorMessageColor"}, $_, color("reset"));
      }
      elsif (m/The file must be an object file, an import file, or an/)
      {
          print($colors{"errorMessageColor"}, $_, color("reset"));
      }
      elsif (m/\(I\) INFORMATION:/)
      {
         print($colors{"noteMessageColor"}, "$_", color("reset"));
      }
      else # Anything else.
      {
         # Doesn't seem to be a warning or an error. Print normally.
         print(color("reset"), $_);
      }
   }
}
elsif ($progName eq "aCC" || $^O eq "hpux")
{
   # Colorize the output from the compiler.
   while(<GCCOUT>)
   {
      # "test.cpp", line 7: warning #2837-D: omission of explicit type is
      # nonstandard
      # "test.c", line 8: remark #2177-D: variable "str" was
      # declared but never referenced
      # "test.c", line 9: error #2077-D: this declaration has no
      # storage class or type specifier
      if (m/^(\".+?\",)( line [0-9]+:)(.+)$/) # filename:lineno:message
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";

         if ($field3 =~ m/error #/)
         {
            print($colors{"errorFileNameColor"}, "$field1", color("reset"));
            print($colors{"errorNumberColor"}, "$field2", color("reset"));
            print($colors{"errorMessageColor"}, "$field3\n", color("reset"));
            while(<GCCOUT>)
            {
               if (m/^$/)
               {
                  last;
               }
               print($colors{"srcColor"}, $_, color("reset"));
            }
         }
         else
         {
            print($colors{"warningFileNameColor"}, "$field1", color("reset"));
            print($colors{"warningNumberColor"}, "$field2", color("reset"));
            print($colors{"warningMessageColor"}, "$field3\n", color("reset"));
            while(<GCCOUT>)
            {
               if (m/^$/)
               {
                  last;
               }
               print($colors{"srcColor"}, $_, color("reset"));
            }
         }
         print("\n");
      }
      # 5 errors detected in the compilation of "test.cpp".
      elsif (m/^[0-9]+ errors detected in the compilation of .+$/)
      {
         print($colors{"errorMessageColor"}, $_, color("reset"));
      }
      # ld: Mismatched ABI (not an ELF file) for test.mk
      # Fatal error.
      elsif (m/ld: Mismatched ABI/)
      {
         print($colors{"errorMessageColor"}, $_, color("reset"));
      }
      elsif (m/Fatal error/)
      {
         print($colors{"errorMessageColor"}, $_, color("reset"));
      }
      # aCC: warning 930: the +Orelaxed_debug option is not valid at this
      # opt_level.
      elsif (m/^aCC: warning \d+: .+$/)
      {
         print($colors{"warningMessageColor"}, $_, color("reset"));
      }
      else # Anything else.
      {
         # Doesn't seem to be a warning or an error. Print normally.
         print(color("reset"), $_);
      }
   }
}
elsif ($progName eq "icc" || $progName eq "icpc")
{
   # Colorize the output from the compiler.
   while(<GCCOUT>)
   {
      # icc on linux uses gnu ld. The following are gnu ld messages
      # /tmp/iccYHwrGF.o: In function `ff()':
      # 4.cpp:(.text+0x34): undefined reference to `gg()'
      if (m/^(.+?:)( .+?`.+?':)$/)
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         srcscan($field1, $colors{"noteFileNameColor"});
         srcscan("$field2\n", $colors{"noteMessageColor"});
      }
      # test.cpp(6): error: identifier "c" is undefined
      elsif (m/^(.+?)(\(\d+\):)(.+)$/) # filename:lineno:message
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         my $field3 = $3 || "";

         if ($field3 =~ m/remark/)
         {
            print($colors{"warningFileNameColor"}, "$field1", color("reset"));
            print($colors{"warningNumberColor"}, "$field2", color("reset"));
            print($colors{"warningMessageColor"}, "$field3\n", color("reset"));
            while(<GCCOUT>)
            {
               if (m/^$/)
               {
                  last;
               }
               print($colors{"introMessageColor"}, $_, color("reset"));
            }
         }
         else
         {
            print($colors{"errorFileNameColor"}, "$field1", color("reset"));
            print($colors{"errorNumberColor"}, "$field2", color("reset"));
            print($colors{"errorMessageColor"}, "$field3\n", color("reset"));
            while(<GCCOUT>)
            {
               if (m/^$/)
               {
                  last;
               }
               print($colors{"introMessageColor"}, $_, color("reset"));
            }
         }
         print("\n");
      }
      # icc: error #10236: File not found:  '4.cpp'
      elsif (m/^(icc|icpc): error #\d+: .+$/)
      {
         print($colors{"errorMessageColor"}, "$_", color("reset"));
      }
      elsif (m/^(.+?:)(.+)$/)
      {
         my $field1 = $1 || "";
         my $field2 = $2 || "";
         srcscan($field1, $colors{"errorFileNameColor"});
         srcscan("$field2\n", $colors{"errorMessageColor"});
      }
      else # Anything else.
      {
         # Doesn't seem to be a warning or an error. Print normally.
         print(color("reset"), $_);
      }
   }
}
else
{
   while(<GCCOUT>)
   {
      print(color("reset"), $_);
   }
}

if ($compiler_pid)
{
   # Get the return code of the compiler and exit with that.
   waitpid($compiler_pid, 0);
   exit ($? >> 8);
}
