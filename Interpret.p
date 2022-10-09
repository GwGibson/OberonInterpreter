program interpret( input, output, infile);

(* P-code interpreter  
   input: text file, 3 integers per line, separated by one or more blanks
*)

const
  header     = 'P-code interpreter';
  inbuffsize = 81;   (* input line length                            *)
  codesize   = 512;  (* length of code array                         *)
  addrmax    = 1023; (* maximum address                              *)
  levelmax   = 7;
   
type
  addrrange = -addrmax .. addrmax;                               
  coderange = 0 .. codesize;
  levrange  = 0 .. levelmax;
  opcodes   = ( hlt, ret, neg, add, sub, imul, idiv, imod, 
                eq, ne, lt, le, gt, ge, orl, andl, notl,
                rdi, rdl, wri, wrl, iabs, isqr,  
                lod, lodc, loda, lodi, sto, stoi, isp,
                jmp, jmpc, call, for0, for1, nop);

  (* instruction set                           

     addresses of form (l, a) where
       l is (static) level difference, and
       a is offset (from BP) within stack frame

     hlt  0, 0 - halt
     ret  0, 0 - return from procedure or function
     neg  0, 0
     add  0, 0
     sub  0, 0
     imul 0, 0
     idiv 0, 0
     imod 0, 0
     eq   0, 0
     ne   0, 0
     lt   0, 0
     le   0, 0
     gt   0, 0
     ge   0, 0
     orl  0, 0
     andl 0, 0
     notl 0, 0
     rdi  0, 0 - read integer, address on TOS
     rdl  0, 0 - readln
     wri  0, 0 - write integer on TOS
     wrl  0, 0 - writeln
     iabs 0, 0
     isqr 0, 0
     lod  l, a - push variable (l, a) on stack
     lodc 0, a - push constant a
     lada l, a - push address of variable (l, a)
     lodi 0, a - push indirect, address of variable at (l, a)
     sto  l, a - pop into variable (l, a)
     popi l, a - pop indirect, address of variable at (l, a)
     call l, a - call procedure/function a, l levels up 
     isp  0, a - increment stack pointer by a  
     jmp  0, a - jump to address a                     
     jmpc l, a - jump conditional to address a ( l = 0: false; l = 1: true)
     for0 l, a - jump to address a if zero trips (l = 0: to; l = 1: downto)
     for1 l, a - jump to address a if repeated trip
     nop  0, 0
  *)

  vminstr = packed record
              op: opcodes;  (* operation code                           *)
              ld: levrange; (* static level difference                  *)
              ad: addrrange (* relative displacement within stack frame *)
            end;
  codearray = packed array[ coderange] of vminstr;

var
  ch:         char;    (* last character read from source file *)
  inbuff:     array[ 1 ..inbuffsize] of char; (* current line  *)
  infile:     text;
  nl:         char; (* new line character *)
   

  mnemonic: array[ opcodes] 
              of packed array[ 1 .. 4] of char;

  code: codearray;        (* storage for machine instructions *)
  lc:   coderange;        (* location counter                 *)
  oplist: array[ 0 .. 40] of opcodes;
  digits,
  letters,
  separators: set of char;

(* ********************* interpreter initialization **********************)

  procedure initinterp;
    
    procedure initscalars;       
    begin (* initscalars *)
      nl     := chr( 10);
    end; (* initscalars *)

    procedure initsets;
    begin (* initsets *)
      digits          := [ '0' .. '9'];
      letters         := [ 'a' .. 'z', 'A' .. 'Z'];
      separators      := [' ', nl];
    end; (* initsets *)
 
    procedure initfiles;
    const
      filenmsize = 20;
    var
      filename:   packed array[ 1.. filenmsize] of char;
      i,
      j: integer;
    begin (* initfiles *)
      writeln;
      write( 'sourcefile: ');
      i := 1;
      while (not eoln) and (i <= filenmsize) do
        begin
          read( ch);
          filename[ i] := ch;
          i := i + 1
        end;
      for j := i to filenmsize do
        filename[ j] := chr( 0);
      writeln;
      assign( infile, filename);
      reset( infile)        
    end; (* initfiles *)

    procedure initinstrmnemonics;
    begin
      mnemonic[ hlt ] := 'hlt ';
      mnemonic[ ret ] := 'ret ';
      mnemonic[ neg ] := 'neg ';
      mnemonic[ add ] := 'add ';
      mnemonic[ sub ] := 'sub ';
      mnemonic[ imul] := 'imul';
      mnemonic[ idiv] := 'idiv';
      mnemonic[ imod] := 'imod';
      mnemonic[ eq  ] := 'eq  ';
      mnemonic[ ne  ] := 'ne  ';
      mnemonic[ lt  ] := 'lt  ';
      mnemonic[ le  ] := 'le  ';
      mnemonic[ gt  ] := 'gt  ';
      mnemonic[ ge  ] := 'ge  ';
      mnemonic[ orl ] := 'orl ';
      mnemonic[ andl] := 'andl';
      mnemonic[ notl] := 'notl';
      mnemonic[ rdi ] := 'rdi ';
      mnemonic[ rdl ] := 'rdl ';
      mnemonic[ wri ] := 'wri ';
      mnemonic[ wrl ] := 'wrl ';
      mnemonic[ iabs] := 'iabs';
      mnemonic[ isqr] := 'isqr';
      mnemonic[ lod ] := 'lod ';
      mnemonic[ lodc] := 'lodc';
      mnemonic[ loda] := 'loda';
      mnemonic[ lodi] := 'lodi';
      mnemonic[ sto ] := 'sto ';
      mnemonic[ stoi] := 'stoi';
      mnemonic[ call] := 'call';
      mnemonic[ isp ] := 'isp ';
      mnemonic[ jmp ] := 'jmp ';
      mnemonic[ jmpc] := 'jmpc';
      mnemonic[ for0] := 'for0';
      mnemonic[ for1] := 'for1';
      mnemonic[ nop ] := 'nop '
    end; (* initinstrmnemonics *)

    procedure initoplist;
    begin
      oplist[  0] := hlt;
      oplist[  1] := ret;
      oplist[  2] := neg;
      oplist[  3] := add;
      oplist[  4] := sub;
      oplist[  5] := imul;
      oplist[  6] := idiv;
      oplist[  7] := imod;
      oplist[  8] := eq;
      oplist[  9] := ne;
      oplist[ 10] := lt;
      oplist[ 11] := le;
      oplist[ 12] := gt;
      oplist[ 13] := ge;
      oplist[ 14] := orl;
      oplist[ 15] := andl;
      oplist[ 16] := notl;
      oplist[ 17] := rdi;
      oplist[ 18] := rdl;
      oplist[ 19] := wri;
      oplist[ 20] := wrl;
      oplist[ 21] := iabs;
      oplist[ 22] := isqr;
      oplist[ 23] := lod;
      oplist[ 24] := lodc;
      oplist[ 25] := loda;
      oplist[ 26] := lodi;
      oplist[ 27] := sto;
      oplist[ 28] := stoi;
      oplist[ 29] := call;
      oplist[ 30] := isp;
      oplist[ 31] := jmp;
      oplist[ 32] := jmpc;
      oplist[ 33] := for0;
      oplist[ 34] := for1;
      oplist[ 35] := nop
    end;

    procedure inputcode;
    var
      n: integer;
      inptr: integer; 

      procedure  getline;
      begin (* getline *)
        inptr := 0;
        while not eoln( infile) do
          begin
            inptr := inptr + 1;
            read( infile, ch);
            inbuff[ inptr] := ch
          end;
        inptr := inptr + 1;
        read( infile, ch); (* read eoln char; ch contains a blank *)
        inbuff[ inptr] := nl; (* store nl character in inbuff     *)
        inptr := 0
      end; (* getline *)
   
      procedure nextchar;
      begin (* nextchar *)
        inptr := inptr + 1;
        ch := inbuff[ inptr]
      end; (* nextchar *)
      
      procedure skipsep;
      begin (* skipsep *)
        while ch in separators do
          nextchar
      end; (* skipsep *)
   
      procedure getnum( var i: integer);
      var
        sign: integer;
      begin (* getnum *)
        skipsep;
        sign := 1;
        if ch = '-' then
          begin
            sign := -1;
            nextchar
          end;
        i := 0;
        repeat
          i := 10 * i + ord( ch) - ord( '0');
          nextchar
        until not (ch in digits);
        i := sign * i
      end; (* getnum *)
       
    begin (* inputcode *)
      lc := 0;
      while not eof( infile) do
        begin
          getline;
          getnum( n);
          with code[ lc] do
            begin
              op := oplist[ n];
              getnum( n);
	      ld := n;
              getnum( n);
              ad := n
            end;
	  with code[ lc] do
            writeln( lc: 3,': ', mnemonic[ op], ' ', ld,' ', ad);
          lc := lc + 1
        end
    end; (* inputcode *)

  begin (* initinterp *)
    writeln;
    writeln( header);
    initscalars;
    initsets;
    initfiles;
    initinstrmnemonics;
    initoplist;
    inputcode
  end; (* initinterp *)

(* ****************** end of interpreter initialization ******************** *)

(* ********************* intermediate code interpreter ********************* *)

  procedure interp;
  const
    stkmax = 1023; (* maximum size of data store *)
  type
    address = 0 .. stkmax;
  var
    ip: coderange;                    (* instruction  pointer                *)
    bp,                               (* frame pointer                       *)
    sp: address;                      (* top of stack pointer                *)
    ir: vminstr;                      (* instruction buffer                  *)
    ps: ( run, stop, divchk, stkchk,
          caschk);                    (* processor status register           *)
    xstack: array[ address] of        (* run time stack                      *)
                 integer;                                            
   
  (* stack structure                                             
   
       |                | <--- last element in previous stack frame
       +----------------+                                        
       |  func ret val  |
       +----------------+
       |   parameters   |
       |                |
       +----------------+
       |  static link   |                     
       +----------------+ |    direction of growth
       | return address | V                                       
       +----------------+ 
       |  dynamic link  | <--- base pointer                            
       | (old base ptr) |
       +----------------+ 
       |     local      |                                        
       |   variables    |
       +----------------+
       |  temporaries   | <--- stack pointer
  *)

    opcount: integer; (* number of operations                                *)
    t1: integer;
    t2: address;
                    
    procedure inter;
     
    function fbase( levdiff: levrange) :address;
    (* find frame base levdiff levels down on static chain                   *)
    var
      tmpptr: address;
    begin (* fbase *)
      tmpptr := bp - 2;
      while levdiff > 0 do
        begin
          tmpptr := xstack[ tmpptr];
          levdiff := levdiff - 1
        end;
      fbase := tmpptr +2
    end; (* fbase *)

    procedure pushac;
    (* push new activation record on stack                                   *)
    begin (* pushac *)
      sp := sp + 1;
      xstack[ sp] := fbase( ir.ld) - 2;      (* store static link            *)
      xstack[ sp + 1] := ip;                 (* store return address         *)
      xstack[ sp + 2] := bp;                 (* store dynamic link           *)
      sp := sp + 2;
      bp := sp                               (* new frame pointer            *)
    end; (* pushac *)

    procedure popac;
    (* pop activation record off stack *)
    begin (* popac *)
      sp := bp;              (* set sp to base of current frame              *)
      ip := xstack[ sp - 1]; (* restore previous ip                          *)
      bp := xstack [ sp];    (* get bp for previous frame                    *)
      sp := sp - 3           (* restore sp to top of previous frame          *)
    end; (* popac *)
   
  begin (* inter *)
    with ir do
      case op of
        hlt : ps := stop;
        ret : popac; (* proc/func return, pop activation record off stack     *)
        neg : xstack[ sp] := - xstack[ sp];
        add : begin
                sp := sp - 1;
                xstack[ sp] := xstack[ sp] + xstack[ sp + 1]
              end;
        sub : begin
                sp := sp - 1;
                xstack[ sp] := xstack[ sp] - xstack[ sp + 1]
              end;
        imul: begin
                sp := sp - 1;
                xstack[ sp] := xstack[ sp] * xstack[ sp + 1]
              end;
        idiv: begin
                sp := sp - 1;
                if xstack[ sp + 1] = 0 then 
                  ps := divchk
                else
                  xstack[ sp] := xstack[ sp] div xstack[ sp + 1]
              end;
        imod: begin
                sp := sp - 1;
                if xstack[ sp + 1] = 0 then
                  ps := divchk
                else
                  xstack[ sp ] := xstack[ sp] mod xstack[ sp + 1]
              end;
        eq  : begin
                sp := sp - 1;
                xstack[ sp] := ord( xstack[ sp] = xstack[ sp + 1])
              end;
        ne  : begin
                sp := sp - 1;
                xstack[ sp] := ord( xstack[ sp] <> xstack[ sp + 1])
              end;
        lt  : begin
                sp := sp - 1;
                xstack[ sp] := ord( xstack[ sp] < xstack[ sp + 1])
              end;
        le  : begin
                sp := sp - 1;
                xstack[ sp] := ord( xstack[ sp] <= xstack[ sp + 1])
              end;
        gt  : begin
                sp := sp - 1;
                xstack[ sp] := ord( xstack[ sp] > xstack[ sp + 1])
              end;
        ge  : begin
                sp := sp - 1;
                xstack[ sp] := ord( xstack[ sp] >= xstack[ sp + 1])
              end;
        orl : begin
                sp := sp - 1;
                if xstack[ sp + 1] = 1 then
                   xstack[ sp] := 1
              end;
        andl: begin
                sp := sp - 1;
                if xstack[ sp + 1] = 0 then
                  xstack[ sp] := 0
              end;
        notl: xstack[ sp] := 1 - xstack[ sp];
        rdi : begin (* rdi - read integer, address on TOS *)
                writeln;
                write( 'Program Input: ');
                readln( xstack[ xstack[ sp]]);
                sp := sp - 1
              end; (* rdi *)
        rdl : readln;
        wri : begin (* wri - write integer on TOS         *)
                writeln; 
		write( 'Program Output: ');
		writeln( xstack[ sp]);
		sp := sp -1
              end;
        wrl : writeln;
        iabs: begin (* iabs *)
                t1 := xstack[ sp];
                if t1 < 0 then
                  xstack[ sp] := -t1
              end;  (* iabs *)
        isqr: begin (* sqr *)
                t1 := xstack[ sp];
                xstack[ sp] := t1 * t1
              end;  (* sqr *)
        lod : begin  
                sp := sp + 1;
                if sp > stkmax then
                  ps := stkchk
                else
                  xstack[ sp] := xstack[ fbase( ld) + ad]
              end;
        lodc: begin (* push constant *)
                sp := sp + 1;
                if sp > stkmax then
                  ps := stkchk
                else
                  xstack[ sp] := ad
                end;
        loda: begin (* push address *)
                sp := sp + 1;
                if sp > stkmax then
                  ps := stkchk
                else
                  xstack[ sp] := fbase( ld) + ad
              end;
        lodi: begin (* push indirect *)
                sp := sp + 1;
                if sp > stkmax then
                  ps := stkchk
                else
                  xstack[ sp] := xstack[ xstack[ fbase( ld) + ad] ]
              end;
        sto :  begin
                xstack[ fbase( ld) + ad] := xstack[ sp];
                sp := sp - 1
              end;
        stoi: begin (* pop indirect *)
                xstack[ xstack[ fbase( ld) + ad] ] := xstack[ sp];
                sp := sp - 1
              end;
        call:  (* jump to procedure or function *)
              if sp + 3 > stkmax then
                ps := stkchk
              else
                begin
                  pushac;  (* push new activation record onto stack          *)
                  ip := ad (* set ip to entry point of proc                  *)
                end;
        isp:  (* increment stack pointer - allocate stack space              *)
              if sp + ad > stkmax then
                ps := stkchk
              else
                sp := sp + ad;
        jmp:  ip := ad;
        jmpc: begin (* jump on condition                                      *)
                if xstack[ sp] =  ld then
                  ip := ad;
                sp := sp - 1
              end;
        for0: begin (* for loop zero trips check                              *)
                t1 := xstack[ sp - 1];
                xstack[ xstack[ sp - 2]] := t1;
                if ld = 0 then
                  begin
                    if t1 > xstack[ sp] then
                      begin
                        ip := ad;
                        sp := sp - 3
                      end
                  end
                else
                  if t1 < xstack[ sp] then
                    begin
                      ip := ad;
                      sp := sp - 3
                    end                                 
              end;
        for1: begin (* for loop repeat trip check                             *)
                t2 := xstack[ sp - 2];
                t1 := xstack[ t2];
                if ld = 0 then
                  begin
                    if t1 < xstack[ sp] then
                      begin
                        xstack[ t2] := t1 + 1;
                        ip := ad
                      end
                    else
                      sp := sp - 3
                  end
                else
                  begin
                    if t1 > xstack[ sp] then
                      begin
                        xstack[ t2] := t1 - 1;
                        ip := ad
                      end
                    else
                      sp := sp - 3
                  end
              end;
        nop:
      end (* case op *)
    end; (* inter *)

    procedure liststack;
    var
      isp: address;
    begin (* liststack *)
      writeln;
      for isp := 0 to sp do
        writeln( isp: 3, ': ', xstack[ isp])
    end; (* liststack *)
  
    procedure pmdump;
    begin (* pmdump *)
      writeln;
      write( 'processor halt at ip = ', ip: 5, ' due to ');
      case ps of
        divchk: writeln( 'division by zero');
        stkchk: writeln( 'run-time stack overflow')
      end (* case *)
    end; (* pmdump *)
   
  begin (* interp *)
    writeln;
    writeln( 'Oberon-S execution');
    (* initialize stack    *)
    xstack[ 0] := 0;                    (*        +---------+        *)
    xstack[ 1] := 0;                    (*        |    0    |        *)
    xstack[ 2] := 0;                    (*        +---------+        *)
    sp := 2;                            (*        |    0    |        *)
    bp := 2;                            (*        +---------+        *)
    opcount := 0;                       (*  sp -> |    0    | <- bp  *)
    ps := run;                          (*        +---------+        *)
    ip := 0;
    repeat 
      ir := code[ ip]; (* fetch instruction   *)
      ip := ip + 1;
      opcount := opcount + 1;
      inter;           (* execute instruction *)
      liststack;
    until ps <> run;
    if ps <> stop then
      pmdump
    else
      begin
        writeln;
        writeln( 'End of execution');
        writeln( opcount: 6, ' operations')
      end
  end; (* interp *)

begin (* interpret *)
  initinterp;
  interp 
end. (* interpret *)         
