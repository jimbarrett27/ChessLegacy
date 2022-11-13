#include <vector>
#include <string>
#include <cmath> // necessary library includes

using namespace std;

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Box.H>
#include <FL/FL_GIF_Image.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Value_Output.H>
#include <FL/Fl_Output.H>//FLTK libraries

class square;
class piece;//two necessary prototypes since these two contain instances of one another

Fl_Window* mywindow;
Fl_Button* buttons[9][9];
Fl_Value_Output* sidebar[5];
Fl_Button* start;
Fl_Box* turnbox;
Fl_Output* info; //Global FLTK objects, defined later

Fl_GIF_Image* pictures[13];//Global FLTK to store pictures of pieces (and a null picture)  



/*******************************************************************

A class to define a piece on the board. This class is parent to a series of subclasses that define the specific rules for movement
and subtleties associated with each individual type of piece. This class also becomes useful for passing coordinates between functions 

********************************************************************/

class piece{

	public:
	
		int rank;
		int file;//coordinates
		
		piece(int rankin=0, int filein=0);
		
		int getrank();
		int getfile();//function prototypes to return coordinates seperately
		
		virtual bool move(piece destination, vector<vector<square> > location, string turn){}	//virtual function to be overwritten for each subclass, 
																								//returning move legality
		virtual piece* changepos(int rankin, int filein){}	//function to change intrinsically defined location of each piece on the board, some subtleties
															//make it innappropriate to have one function encompassing all subclasses, so virtual used.
		virtual string myname(){}//virtual function, for which each subclass will return its name
};

piece::piece(int rankin, int filein) {//piece constructor

	rank = rankin;
	file = filein;
	
}

int piece::getrank(){return rank;}
int piece::getfile(){return file;}//trivial functions returning coordinate info



/*******************************************************************

creating square class, containing useful information about a board square
including whether the square is occupied, the colour of that piece, and a pointer to the piece object itself

*******************************************************************/		

class square {

	private:
	
		bool occupied;
		string colour;
		piece* pieceon;
		
	public:
	
		square(bool occupiedin=false, string colourin="empty", piece* pieceonin=NULL); 
		
		bool isoccupied();
		string colourocc();
		piece* getpiece();//trivial function prototypes to return square's properties individually
		
};

square::square(bool occupiedin, string colourin, piece* pieceonin) {//square constructor

	occupied = occupiedin;
	colour = colourin;
	pieceon = pieceonin;
	
}

bool square::isoccupied() {return occupied;}
string square::colourocc() {return colour;}
piece* square::getpiece() {return pieceon;}//simple functions to return square's seperate properties when called



/*******************************************************************

Prototypes for all functions (defined in full after main()). 
Only a brief description of each given here, full information is give with the function code itself. 

********************************************************************/

bool diagonal_impeded(int begin_rank, int end_rank, int begin_file, int end_file, vector<vector<square> > location);
//returns whether any square is occupied on a diagonal between two squares

bool rank_impeded(int begin, int end, int file, vector<vector<square> > location);
//returns whether any square is occupied between squares along a rank

bool file_impeded(int begin, int end, int rank, vector<vector<square> > location);
//returns whether any square is occupied between squares along a file

bool legalmove_diagonal(int beginrank, int beginfile, int endrank, int endfile, vector<vector<square> > location, string turn);
//returns whether or not a diagonal move is legal
bool legalmove_rank(int beginrank, int beginfile, int endrank, int endfile, vector<vector<square> > location, string turn);
//returns whether or not a move along a rank is legal

bool legalmove_file(int beginrank, int beginfile, int endrank, int endfile, vector<vector<square> > location, string turn);
//returns whether or not a move along a file is legal

string changeturn(string turn);
//returns the opposite turn to the argument

void drawboard();
//draws an array of FLTK buttons to form the board

bool maketurn(piece from, piece to);
//links together all the functions checking that a move is legal, puts anyone in check or checkmate etc. also
//updates the global board with any changes

vector<vector<square> > resetboard();
//clears the board and sets all the pieces up in the starting positions

void drawpieces(vector<vector<square> > currentboard);
//looks through all the squares, and where it finds a piece, puts a picture of that piece on that square

bool check(string turn, vector<vector<square> > board);
//finds if the board and pieces are in a state of check

string currentturn = "white";
//global string to keep track of whose turn it is

vector<vector<square> > currentboard(9, vector<square> (9));
//global 2D vector of squares to be the board. Dimensions are 9x9 so that coding coordinates are intuitive
//and don't start counting at 0.  

vector<vector<square> > movingpieces(vector<vector<square> > tempboard, string turn, int frank, int ffile, int trank, int tfile);
//contains the code necessary for changing the piece positions, the properties of
//the squares, and creates any new pieces necessary.

bool identicalboards(vector<vector<square> > board1, vector<vector<square> > board2);
//compares whether or not two 9x9 arrays of squares are the same

bool mate(vector<vector<square> > tempboard, string turn);
//looks for any legal move whereby the king can avoid being taken


/*******************************************************************

//creating subclass inheriting from the piece class to control pawns, containing the pawn's location, whether it has moved yet
//to accomodate double move rule and whether it can be captured en passant.

*******************************************************************/

class pawn : public piece {

	public:
	
		bool movedyet;
		bool enpassant;
	
		pawn(int rankin=1, int filein=1, bool movedyetin = false, bool enpassantin = true);
		
		bool getmovedyet();//returns whether the piece has moved yet
		bool move(piece destination, vector<vector<square> > location, string turn);//rules for movement
		piece* changepos(int rankin, int filein);//changes intrinsic coordinates
		string myname();//returns name of piece
		bool enpassantable();//returns whether the pawn can be captured en-passant
	
};

pawn::pawn(int rankin, int filein, bool movedyetin, bool enpassantin) {//pawn constructor

	rank = rankin;
	file = filein;
	movedyet = movedyetin;
	enpassant = enpassantin;
	
}

bool pawn::getmovedyet(){return movedyet;}//trivial function returning whether pawn has moved yet

bool pawn::enpassantable(){return enpassant;}//trivial function returning whether the pawn can be taken en passant

bool pawn::move(piece destination, vector<vector<square> > location, string turn){	      	     
	
		if (turn=="white"){ //easier to divide into rules dependent on colour, since pawns only move one way
		
			if (destination.getrank()-rank==2//if moving 2 squares along rank 
			&& movedyet==false // and pawn hasn't moved yet
			&& location[destination.getrank()][destination.getfile()].isoccupied()==false//and target square is empty
			&& destination.getfile()==file)// and no sideways movement  
			return true;//if pawn's first move then let it move 2 spaces
			
			else if ((destination.getrank()-rank)==1//if move is one square forward 
			&& abs(destination.getfile()-file)==1//and one accross (only way pawns can capture normally) 
			&& location[destination.getrank()][destination.getfile()].isoccupied()==true//make sure a piece is present 
			&& location[destination.getrank()][destination.getfile()].colourocc()!=currentturn)//and it's not own piece
			return true;//rule for pawns capturing
			
			else if (destination.getrank()-rank == 1
			&& abs(destination.getfile()-file) == 1//same capture move as before with extra for en-passant rule
			&& location[destination.getrank()-1][destination.getfile()].isoccupied()==true 
			&& location[destination.getrank()-1][destination.getfile()].getpiece()->myname()=="pawn"//en passant can only capture pawns 
			&& location[destination.getrank()-1][destination.getfile()].colourocc()!=turn)
			{
				pawn * EP = (pawn*)location[destination.getrank()-1][destination.getfile()].getpiece();	//need to regain the pawn from the more general piece* variable
																										//normally contained in the square, on the en-passant capture square
				if (EP->enpassantable() == true) return true;//legal move if that pawn can be captured en-passant
				else return false;//otherwise illegal
			}
	
			else if (destination.getrank()-rank==1//if move one square forward 
			&& location[destination.getrank()][destination.getfile()].isoccupied()==false//to an empty square
			&& destination.getfile()==file)//along same file
			return true;
	
			else return false;//if move illegal return piece from whence it came
		
		}
		
		if (turn=="black"){// all black moves are identical, except with negative numbers indicating movement in the opposite direction to white 
		
			if ((destination.getrank()-rank)==-2 
			&& movedyet==false
			&& location[destination.getrank()][destination.getfile()].isoccupied()==false
			&& destination.getfile()==file)  
			return true;//if pawn's first move then let it move 2 spaces
	
			else if ((destination.getrank()-rank)==-1 
			&& abs(destination.getfile()-file)==1
			&& location[destination.getrank()][destination.getfile()].isoccupied()==true 
			&& location[destination.getrank()][destination.getfile()].colourocc()!=turn)
			return true;//rule for pawns capturing
			
			else if (destination.getrank()-rank == -1
			&& abs(destination.getfile()-file) == 1
			&& location[destination.getrank()+1][destination.getfile()].isoccupied()==true
			&& location[destination.getrank()+1][destination.getfile()].getpiece()->myname()=="pawn" 
			&& location[destination.getrank()+1][destination.getfile()].colourocc()!=turn)
			{
				pawn * EP = (pawn*)location[destination.getrank()+1][destination.getfile()].getpiece();
				if (EP->enpassantable() == true) return true;
				else return false;//en passant
			}
			
			else if ((destination.getrank()-rank)==-1 
			&& location[destination.getrank()][destination.getfile()].isoccupied()==false
			&& destination.getfile()==file)
			return true;//if none of the above, check it satisfies basic 1 square pawn move
	
			else return false;//if move illegal return piece from whence it came
		
		}

}

piece* pawn::changepos(int rankin, int filein){

	piece * out = new pawn(rankin,filein,true,false); 
	return out;
	
}//changes the intrinsic coordinates, as well as updating that the pawn has now been moved. En-passant is considered in movingpieces() function

string pawn::myname(){return("pawn");}//returns that its a pawn

/*******************************************************************

A class to describe the knight, inheriting from the piece class. The knight is quite a simple beast, and only requires the properties inherited
from piece.

*******************************************************************/

class knight : public piece {
	
	public:
	
		knight(int rankin=1, int filein=1);
		
		bool move(piece destination, vector<vector<square> > location, string turn);
		piece* changepos(int rankin, int filein);
		string myname();
	
		
};

knight::knight(int rankin, int filein) {//knight constructor

	rank = rankin;
	file = filein;
	
}

bool knight::move(piece destination, vector<vector<square> > location, string turn) {

	//2 simple conditionals describing the L-shape of legal knight moves for when the destination square is empty 
	
	if (abs(destination.getfile()-file) == 1
	&& abs(destination.getrank()-rank) == 2
	&& location[destination.getrank()][destination.getfile()].isoccupied()==false)
	return true;
	
	else if (abs(destination.getfile()-file) == 2
	&& abs(destination.getrank()-rank) == 1
	&& location[destination.getrank()][destination.getfile()].isoccupied()==false)
	return true;
	
	//two more similar functions, except this time for where the destination is occupied
	
	else if (abs(destination.getfile()-file) == 1
	&& abs(destination.getrank()-rank) == 2
	&& location[destination.getrank()][destination.getfile()].colourocc()!=turn)
	return true;
	
	else if (abs(destination.getfile()-file) == 2
	&& abs(destination.getrank()-rank) == 1
	&& location[destination.getrank()][destination.getfile()].colourocc()!=turn)
	return true;
	
	else return false;//if move is illegal
}

piece* knight::changepos(int rankin, int filein){

	piece * out = new knight(rankin,filein); 
	return out;
	
}//standard changepos(), changing intrinsic coordinates

string knight::myname(){return("knight");}//returns that its a knight

/*******************************************************************

A class to describe the Rook inheriting from piece, similar to the pawn in that in needs to keep track 
of whether it has moved yet. 

*******************************************************************/

class rook : public piece {
	
	private:
	
		bool movedyet;//true if rook has already moved, false otherwise
	
	public:
	
		rook(int rankin=1, int filein=1,bool movedyetin=false);
		bool getmovedyet();
		bool move(piece destination, vector<vector<square> > location, string turn);
		piece* changepos(int rankin, int filein);
		string myname();//all these functions are the same as with the pawn subclass
	
		
		
};

rook::rook(int rankin, int filein,bool movedyetin) {//rook constructor

	rank = rankin;
	file = filein;
	movedyet = movedyetin;
	
}

bool rook::move(piece destination, vector<vector<square> > location, string turn){

	rook initial(rank,file,movedyet);
	
	if (legalmove_rank(rank, file, destination.getrank(), destination.getfile(),location,turn) == true
	|| legalmove_file(rank, file, destination.getrank(), destination.getfile(),location,turn) == true)
	return true;//simply calls other functions determining whether it is legal to move along the rank or file 
	
	else return false;//otherwise returns that the move is illegal
}

bool rook::getmovedyet(){return movedyet;}//trivial function returning whether the rook has moved yet

piece* rook::changepos(int rankin, int filein){

	piece * out = new rook(rankin,filein,true); 
	return out;
	
}//changes the intrinsic coordinates, as well as updating that the piece has been moved now

string rook::myname(){return("rook");}//returns that it's a rook



/*******************************************************************

A class describing the King, inheriting from the piece class. Whilst the rules for the king's movement 
are probably the most complicated, it still only needs one extra piece of information, whether it has moved yet,
the functions of which are identical to the rook and pawn.

*******************************************************************/

class king : public piece {

	private:
	
		bool movedyet;
	
	public:
	
		king(int rankin=1, int filein=1, bool movedyetin = false);
		
		bool getmovedyet();
		bool move(piece destination, vector<vector<square> > location, string turn);
		piece* changepos(int rankin, int filein);
		string myname();//everything as with the rook
	
		
		
};

king::king(int rankin, int filein, bool movedyetin) {//king constructor

	rank = rankin;
	file = filein;
	movedyet = movedyetin;
	
}

bool king::getmovedyet(){return movedyet;}//as with rook

bool king::move(piece destination, vector<vector<square> > location, string turn){

	king initial(rank,file,movedyet);
	
	piece * castle;//piece pointer required to get information about a rook for the castling move
	
	if (location[destination.getrank()][destination.getfile()].isoccupied() == true
	&& location[destination.getrank()][destination.getfile()].colourocc() == turn) return false;
	//simply saying that if the destination square is occupied by a piece of the same colour, then the move is illegal
	
	else if ((abs(destination.getrank()-rank) == 1)//if moving one square along a file
	&& legalmove_file(rank, file, destination.getrank(), destination.getfile(),location,turn) == true)//check it's legal
	return true;
	
	else if ((abs(destination.getfile()-file) == 1)//if moving one square along a rank, 
	&& legalmove_rank(rank, file, destination.getrank(), destination.getfile(),location,turn) == true)//check it's legal
	return true;
	
	else if (abs(destination.getfile()-file) == 1//if moving one square along rank
	&& abs(destination.getrank()-rank) == 1 //aswell as one along a file (diagonal)
	&& legalmove_diagonal(rank, file, destination.getrank(), destination.getfile(),location,turn) == true)//check its legal
	return true;
	
	//following two functions describe castling. Castling caused a lot of issues and bugs in the program which were
	//fixed by adding conditions. These conditions therefore may not be as concise as they could be. It is worth 
	//pointing out that the property movedyet negates the need to specify the 1st and 8th ranks fro white and black
	//respectively. deletion of rook comes later
	
	//castling on the queenside
	else if (abs(destination.getfile()-file) == 2 // if king is moving two squares along rank
	&& destination.getrank() == rank//and none along a file
	&& destination.getfile() == 3 //and destination is correct for queenside castling
	&& location[rank][1].isoccupied() == true //checking square where rook should be is occupied
	&& location[rank][1].getpiece()->myname()=="rook"//and that it is a rook
	&& location[rank][1].colourocc() == turn //and that it's own rook
	&& legalmove_rank(rank, file, destination.getrank(), 3,location,turn) == true//and that the rank is empty between king and rook	  
	&& movedyet==false)//and the king hasn't moved yet
	{	
		rook * castle = (rook*)location[rank][1].getpiece();//convert general piece* into the rook in question
		if (castle->getmovedyet()==false) return true;//and ask if it has moved yet, returning legal move if not
		else return false;
	}
	
	//castling on kingside, holds same logic as queenside, except with different target squares
	else if (abs(destination.getfile()-file) == 2
	&& destination.getrank() == rank
	&& destination.getfile()== 7
	&& location[rank][8].isoccupied() == true
	&& location[rank][8].getpiece()->myname()=="rook"
	&& location[rank][8].colourocc() == turn
	&& legalmove_rank(rank, file, destination.getrank(), 7,location,turn) == true	
	&& movedyet==false)
	{	
		rook * castle = (rook*)location[rank][8].getpiece();
		if (castle->getmovedyet()==false) return true;
		else return false;
	}
	 	
	else return false; //if move otherwise illegal, say so
}

piece* king::changepos(int rankin, int filein){

	piece * out = new king(rankin,filein,true); 
	return out;
	
}//chnages intrinsic coordinates, and updates that king has been moved

string king::myname(){return("king");}//returns that its a king

/*******************************************************************

A class describing the king, inheriting from the piece class. A reasonably simple object, with no new functions
or intrinsic data added to that which it has inherited

*******************************************************************/

class queen : public piece {
	
	public:
	
		queen(int rankin=1, int filein=1);
		
		bool move(piece destination, vector<vector<square> > location, string turn);
		piece* changepos(int rankin, int filein);
		string myname();

};

queen::queen(int rankin, int filein) {//queen constructor

	rank = rankin;
	file = filein;
	
}

bool queen::move(piece destination, vector<vector<square> > location, string turn){

	queen initial(rank,file);
	
	if (legalmove_diagonal(rank, file, destination.getrank(), destination.getfile(),location,turn) == true
	|| legalmove_rank(rank, file, destination.getrank(), destination.getfile(),location,turn) == true
	|| legalmove_file(rank, file, destination.getrank(), destination.getfile(),location,turn) == true)
	return true;//calling functions to check legality of horizontal, vertical and diagonal moves, returning true
	//if any are possible
	
	else return false;
}


piece* queen::changepos(int rankin, int filein){

	piece * out = new queen(rankin,filein); 
	return out;
}//changing intrinsic coordinates

string queen::myname(){return("queen");}//returning that its a queen



/*******************************************************************

A class describing the Bishop, inheriting from the piece class. Another simple beast, only using the properties 
inherited from the piece class.

*******************************************************************/

class bishop : public piece {
	
	public:
	
		bishop(int rankin=1, int filein=1);
		
		bool move(piece destination, vector<vector<square> > location, string turn);
		piece* changepos(int rankin, int filein);
		string myname();
		
};

bishop::bishop(int rankin, int filein) {//bishop constructor

	rank = rankin;
	file = filein;
	
}

bool bishop::move(piece destination, vector<vector<square> > location, string turn){
	
	if (legalmove_diagonal(rank, file, destination.getrank(), destination.getfile(),location,turn) == true)
	return true;//checks if the diagonal move is legal, otherwise returning illegal move.
	
	else return false;
}

piece* bishop::changepos(int rankin, int filein){

	piece * out = new bishop(rankin,filein); 
	return out;
	
}//changes intrinsic coordinates 

string bishop::myname(){return("bishop");}//returns that its a bishop

/******************************************************************* 

Callback function for the newgame button, which resets the program to its initial state

*******************************************************************/

void newgame(Fl_Widget * w) {

	int i, j;
	
	currentboard = resetboard();//put all pieces in starting positions 
	
	drawpieces(currentboard);//put all the pictures of pieces on the board buttons
	
	currentturn="white";// revert game to whites turn
	
	turnbox->color(FL_WHITE);//update turn indicator to white
	turnbox->redraw();//and refresh it
	
	for (j=1; j<9; j++){
	
		for (i=1; i<9; i++){
		
			buttons[i][j]->redraw();//refresh and redraw all 64 buttons
		}
	
	}
	
	info->value(" ");//revert check/checkmate/stalemate info box to blank
	
}

/*************************************************************************

Callback function and its globals to interpret pressing of the buttons that make up the board, send this information
to the fuctions that actually know chess rules, and then redraw the board once a move has been made

*************************************************************************/

piece* where;	//Global object necessary to recieve non integer data type in buttonwhere callback. Gets data from
				//the FLTK buttons drawn in the drawboard() function below
				
bool alternate = true; 	//global variable to make the buttonwhere function take two button pushes (a square to move from
						//and a destination square, before passing those to other functions to check legality etc 

void buttonwhere(Fl_Widget * w, void * where){

	piece * here = (piece*)where;	//converts void* to its more useful original piece type, containing coordinates of which
									//button was pressed from drawboard() function
	
	piece from, to;
	
	int i, j; 
	
	if (alternate==true){//first time button is pressed, alternate is true, so stored coordinates are where the move is from 
	
		sidebar[0]->value(here->getrank());//showing user where they clicked to move from, and storing this info
		sidebar[1]->value(here->getfile());//for when the next button is pressed
		alternate=false;//changing the global so that alternative code is run next time this callback is called
		
	}
	
	else if (alternate==false){//means that this code is run with the push of the destination square
	
		sidebar[2]->value(here->getrank());
		sidebar[3]->value(here->getfile());//displays coordinates of destination square
		alternate = true;
		
		from = piece(sidebar[0]->value(),sidebar[1]->value());//remembers from last button push
		to = piece(sidebar[2]->value(),sidebar[3]->value());//creates suitable variables to send square to move from and destination to the relevant function
		
		if (maketurn(from,to)==true){//calls the function that looks at the overall legality of the move between two squares, returning true if legal
		
			drawpieces(currentboard);//updates which images are to be associated with which board buttons
	
			for (j=1; j<9; j++){
	
				for (i=1; i<9; i++){
		
					buttons[i][j]->redraw();
			
				}
	
			}//cycles through the whole board of buttons and redraws them
			
			if (currentturn=="white"){
			
				turnbox->color(FL_WHITE);
				turnbox->redraw();
			}
			
			else{
			
				turnbox->color(FL_BLACK);
				turnbox->redraw();
			}//changes the turn indicator in the sidebar dependent on whose move it is
		
		}
		
	}
	
}

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

int main() {

	mywindow = new Fl_Window(1200,800,"Chess!");//creates FLTK window
	
	sidebar[0] = new Fl_Value_Output(900, 100, 60, 30, "From ");
	sidebar[1] = new Fl_Value_Output(1000, 100, 60, 30, "");
	sidebar[2] = new Fl_Value_Output(900, 200, 60, 30, "To ");
	sidebar[3] = new Fl_Value_Output(1000, 200, 60, 30, "");//creates the info boxes to show coordinates of chosen
															//and destination square
	
	start = new Fl_Button(900,275,160,100,"New Game");
	start->callback(newgame);//links a new game button to its callback, the purpose of which is to reset the board 
	
	pictures[0] = new Fl_GIF_Image("kingw.gif");
	pictures[1] = new Fl_GIF_Image("kingb.gif");
	pictures[2] = new Fl_GIF_Image("queenw.gif");
	pictures[3] = new Fl_GIF_Image("queenb.gif");
	pictures[4] = new Fl_GIF_Image("rookw.gif");
	pictures[5] = new Fl_GIF_Image("rookb.gif");
	pictures[6] = new Fl_GIF_Image("bishopw.gif");
	pictures[7] = new Fl_GIF_Image("bishopb.gif");
	pictures[8] = new Fl_GIF_Image("knightw.gif");
	pictures[9] = new Fl_GIF_Image("knightb.gif");
	pictures[10] = new Fl_GIF_Image("pawnw.gif");
	pictures[11] = new Fl_GIF_Image("pawnb.gif");
	pictures[12] = new Fl_GIF_Image("null.gif");//importing all images of chess pieces into an array
	
	turnbox = new Fl_Box(930, 425, 100,100);
	turnbox->box(FL_FLAT_BOX);
	turnbox->color(FL_WHITE);//creating a box to indicate whose turn it is, with an initial colour white (since white moves first
	
	info = new Fl_Output(925, 650, 110, 20);
	info->value(" ");//creates a text output box to display things like check/checkmate/stalemate
	
	drawboard();//function call that draws all the buttons
	
	currentboard = resetboard();//function call to set the global board with pieces in their starting positions
	
	drawpieces(currentboard);//associates all the newly created pieces with their pictures and puts them on the board 
	
	mywindow->end();
	mywindow->show();
	
	return Fl::run();//opens the window
	
}

/*******************************************************************/
/*******************************************************************/
/*******************************************************************/
/*******************************************************************/

/********************************************************************

A function that looks at every square along the diagonal between two squares,
checking if any of those squares are occupied 

********************************************************************/ 

bool diagonal_impeded(int begin_rank, int end_rank, int begin_file, int end_file, vector<vector<square> > location){

	int h=0, i=0, j=0, k=0;
	bool impeded=false, flag=false;
	
	if (end_rank - begin_rank > 0) j=1;
	else j=-1;	//decides whether the diagonal move is up or down the board, setting a counter 
				//to increase or decrese respectively
	
	if (end_file - begin_file > 0) k=1;
	else k=-1; //similarly, setting up a counter for left or right overall diagonal movement
	
	if (begin_rank + j == end_rank) return false;//if diagonal move is only one square, it cant be impeded
	
	h = begin_rank;
	i = begin_file;//initialising where the loop should start
	
	while (flag == false){
	
			h+=j;
			i+=k;//stepping one square diagonally (in correct direction)
		
			if (location[h][i].isoccupied()==true) impeded=true;//setting a boolean to true if a piece is found on the square
			
			flag = true;//tripping a flag, see next
			
			if (j==1 && h<(end_rank-1)) flag = false;
			else if (j==-1 && h>(end_rank+1)) flag = false;	//checking if the end of the diagonal in question has been reached
															//continuing the loop if it hasn't  
		
	}
	
	return impeded;//returning true if diagonal has a piece on it, false if its empty 
}

/********************************************************************

A pair of functions to check to see if any squares between two specified squares are occupied.
Both functions have almost identical reasoning and operation, they were left as seperate so as to
keep their use intuitive to read.

*********************************************************************/

bool file_impeded(int begin, int end, int file, vector<vector<square> > location) {

	int i=0, j=0;
	bool impeded=false, flag=false;
	
	if (end - begin >0) j=1;
	else if (end - begin <0) j=-1;//specifying a counter to allow for forward and backwards movement
	
	i = begin;//setting start of counting for loop
	
	if (abs(begin-end) == 1) return false;//if move is only one square, then it cant be an impeded path
	
	while (flag == false){
	
		i+=j;//counting
		
		if (location[i][file].isoccupied()==true) impeded=true;//tripping a boolean if a square is occupied
			
		flag = true;//setting a flag to end loop if conditions arent satisfied
			
		if (j==1 && i<(end-1)) flag = false;
		else if (j==-1 && i>(end+1)) flag = false;//checking if last square reached
	}  	     	 
	
	return impeded;//returnig true if file impeded and false otherwise
	
}  
	 
bool rank_impeded(int begin, int end, int rank, vector<vector<square> > location) {

	int i=0,j=0;
	bool impeded=false, flag = false;
	
	if (end - begin >0) j=1;
	else if (end - begin <0) j=-1;

	i = begin;
	
	if (abs(begin-end)==1) return false;
	
	while (flag == false){
	
		i+=j;
		
		if (location[rank][i].isoccupied()==true) impeded=true;//this line is the one that differs from file_impeded()
			
		flag = true;
			
		if (j==1 && i<(end-1)) flag = false;
		else if (j==-1 && i>(end+1)) flag = false;
	}  	     	 
	
	return impeded;//similar to file_impeded() but for ranks instead
	
}

/****************************************************************************

3 functions that specify the rules associated with long range moves along ranks, files and diagonals
which is necessary for queens, bishops,rooks and useful for kings move functions

****************************************************************************/

bool legalmove_diagonal(int beginrank,int beginfile, int endrank, int endfile, vector<vector<square> > location, string turn){

	if (abs(endrank-beginrank) == abs(endfile-beginfile)//move is actually diagonal
	&& diagonal_impeded(beginrank,endrank,beginfile,endfile,location)==false//and diagonal not impeded
	&& location[endrank][endfile].isoccupied()==false)//and end square vacant
	return true;//diagonal move
	
	else if (abs(endrank-beginrank) == abs(endfile-beginfile)//move diagonal
	&& diagonal_impeded(beginrank,endrank,beginfile,endfile,location)==false//not impeded
	&& location[endrank][endfile].colourocc()!=turn)//piece of opposite colour at destination
	return true;//diagonal capture
	
	else return false;//true if legal, false otherwise
	
} 

bool legalmove_file(int beginrank, int beginfile, int endrank, int endfile, vector<vector<square> > location, string turn) {

	if (endfile == beginfile//move limited to 1 file
	&& file_impeded(beginrank,endrank,beginfile,location)==false//file not impeded
	&& location[endrank][endfile].isoccupied()==false)//destinatio vacant
	return true;//moving along rank
	
	else if (endfile == beginfile//move along file
	&& file_impeded(beginrank,endrank,beginfile,location)==false//file not impeded
	&& location[endrank][endfile].colourocc()!=turn)//piece of opposite colour at destination
	return true;//capturing along rank
	
	else return false;//true if legal, false otherwise
	
}

bool legalmove_rank(int beginrank, int beginfile, int endrank, int endfile, vector<vector<square> > location, string turn) {

	if (endrank == beginrank
	&& rank_impeded(beginfile,endfile,beginrank,location)==false
	&& location[endrank][endfile].isoccupied()==false)
	return true;//moving along rank
	
	else if (endrank == beginrank
	&& rank_impeded(beginfile,endfile,beginrank,location)==false
	&& location[endrank][endfile].colourocc()!=turn)
	return true;//capturing along rank
	
	else return false;//very similar to legalmove_file(), true if legal, false otherwise
	
}

/********************************************************************

fairly trivial function to return the opposite turn to that supplied

********************************************************************/

string changeturn(string turn) {

	if (turn == "white") return("black");
	else if (turn == "black") return("white");
}

/*******************************************************************

Function to draw an array of alternating black (well ...green) and white buttons,
which are linked to the buttonwhere() callback function

*******************************************************************/

void drawboard(){

	int i, j;

	for (j=1; j<9; j++){
	
		if (j%2 == 0){
		
			for (i=1; i<9; i++) {
			
				buttons[i][j] = new Fl_Button(0+(100*(i-1)),700-(100*(j-1)),100,100,"");
				buttons[i][j]->color(FL_WHITE);
				where = new  piece(j,i);
				buttons[i][j]->callback(buttonwhere, where);
				
				i++;
				
				buttons[i][j] = new Fl_Button(0+(100*(i-1)),700-(100*(j-1)),100,100,"");
				buttons[i][j]->color(FL_GREEN);
				where = new piece(j,i);
				buttons[i][j]->callback(buttonwhere, where);
			
			}
			
		}
		
		else if (j%2 == 1) {
		
			for (i=1; i<9; i++) {
	
				buttons[i][j] = new Fl_Button(0+(100*(i-1)),700-(100*(j-1)),100,100,"");
				buttons[i][j]->color(FL_GREEN);
				where = new piece(j,i);
				buttons[i][j]->callback(buttonwhere, where);
				
				i++;
				
				buttons[i][j] = new Fl_Button(0+(100*(i-1)),700-(100*(j-1)),100,100,"");
				buttons[i][j]->color(FL_WHITE);
				where = new piece(j,i);
				buttons[i][j]->callback(buttonwhere, where);
			}
		
		}
	
	}
	
}

/*****************************************************************************

A simple function cycling through all 64 board squares, drawing the relevant peices where it finds them,
other wise drawing a null picture (single white pixel)

*****************************************************************************/
	
void drawpieces(vector<vector<square> > currentboard){

	int i, j;                  

	for (j=1; j<9; j++){
	
		for (i=1; i<9; i++){
		
		if (currentboard[j][i].isoccupied()==true) {
		
			if (currentboard[j][i].getpiece()->myname()=="king" && currentboard[j][i].colourocc()=="white") 
			buttons[i][j]->image(pictures[0]);
			
			else if (currentboard[j][i].getpiece()->myname()=="king" && currentboard[j][i].colourocc()=="black") 
			buttons[i][j]->image(pictures[1]);
			
			else if (currentboard[j][i].getpiece()->myname()=="queen" && currentboard[j][i].colourocc()=="white") 
			buttons[i][j]->image(pictures[2]);
			
			else if (currentboard[j][i].getpiece()->myname()=="queen" && currentboard[j][i].colourocc()=="black") 
			buttons[i][j]->image(pictures[3]);
			
			else if (currentboard[j][i].getpiece()->myname()=="rook" && currentboard[j][i].colourocc()=="white") 
			buttons[i][j]->image(pictures[4]);
			
			else if (currentboard[j][i].getpiece()->myname()=="rook" && currentboard[j][i].colourocc()=="black") 
			buttons[i][j]->image(pictures[5]);
			
			else if (currentboard[j][i].getpiece()->myname()=="bishop" && currentboard[j][i].colourocc()=="white") 
			buttons[i][j]->image(pictures[6]);
			
			else if (currentboard[j][i].getpiece()->myname()=="bishop" && currentboard[j][i].colourocc()=="black") 
			buttons[i][j]->image(pictures[7]);
			
			else if (currentboard[j][i].getpiece()->myname()=="knight" && currentboard[j][i].colourocc()=="white") 
			buttons[i][j]->image(pictures[8]);
			
			else if (currentboard[j][i].getpiece()->myname()=="knight" && currentboard[j][i].colourocc()=="black") 
			buttons[i][j]->image(pictures[9]);
			
			else if (currentboard[j][i].getpiece()->myname()=="pawn" && currentboard[j][i].colourocc()=="white") 
			buttons[i][j]->image(pictures[10]);
			
			else if (currentboard[j][i].getpiece()->myname()=="pawn" && currentboard[j][i].colourocc()=="black") 
			buttons[i][j]->image(pictures[11]);
		
		}
			
		else buttons[i][j]->image(pictures[12]);
		
		}
	}
		
}

/***************************************************************************

A function designed to take in initial and destination coordinates, and then link together all the 
functions necessary to check legality and move the pieces, check for check/checkmate/stalemate and change the globals 
accordingly.

***************************************************************************/


bool maketurn(piece from, piece to){

	int h=0,i=0, j=0, k=0, frank=0, ffile=0, trank=0, tfile=0;
	
	vector<vector<square> > tempboard(9, vector<square> (9));
	
	vector<vector<square> > savedboard(9, vector<square> (9));
	
	tempboard = currentboard;//creates a temporary board to run checks on
	
	string otherturn=changeturn(currentturn);
	
	bool flag = true;
	
	frank=from.getrank();
	ffile=from.getfile();
	
	if (tempboard[frank][ffile].isoccupied()!=true || tempboard[frank][ffile].colourocc()!=currentturn) {
	
		return false;
		
	}//if its a piece you can't move or theres no piece at all, there's no point running any more code
		
	trank=to.getrank();
	tfile=to.getfile();
				
	bool movecheck = tempboard[frank][ffile].getpiece()->move(to,tempboard,currentturn);//calls move function for piece class to be moved	 	 	 	 
		
	if (movecheck==true) {
	
		tempboard = movingpieces(tempboard, currentturn, frank,ffile,trank,tfile);//calls function to move pieces
		
		if (identicalboards(currentboard,tempboard)==true) return false;//later function returns an identical board if theres a problem with the move
				
		if (check(currentturn, tempboard)==true) {
		
			info->value("CHECK!");//displays if it's check
			
			if (mate(tempboard, currentturn)==true) info->value("CHECKMATE!!!");//if it's check and mate, that's checkmate
		
		}
		
		else if (mate(tempboard, currentturn)==true) info->value("STALEMATE");//if its not check but is mate, that's stalemate 	      	    	
		
		else info->value(" ");//otherwise it's nothing
		
		currentturn = changeturn(currentturn);//change turn global
		
		currentboard = tempboard;//if its got this far, move is totally fine and the global board is updated
		
		return true;//true if move OK
	}
	
	else return false;//false if it aint

}

/*****************************************************************************

Function to clear the board and put pieces back into their starting positions

*****************************************************************************/

vector<vector<square> > resetboard(){

	vector<vector<square> > board(9, vector<square> (9));
	
	int i, j;
	
	for (i=1; i<9; i++) {
	
		for (j=1; j<9; j++) {
		
			board[i][j]=square(false,"empty",0);
		}
	}//clearing the board of pieces
	
	piece * P1 = new pawn(2,1,false); board[2][1]=square(true,"white",P1);
	piece * P2 = new pawn(2,2,false); board[2][2]=square(true,"white",P2);
	piece * P3 = new pawn(2,3,false); board[2][3]=square(true,"white",P3);
	piece * P4 = new pawn(2,4,false); board[2][4]=square(true,"white",P4);
	piece * P5 = new pawn(2,5,false); board[2][5]=square(true,"white",P5);
	piece * P6 = new pawn(2,6,false); board[2][6]=square(true,"white",P6);
	piece * P7 = new pawn(2,7,false); board[2][7]=square(true,"white",P7);
	piece * P8 = new pawn(2,8,false); board[2][8]=square(true,"white",P8);
	
	piece * R1 = new rook(1,1,false); board[1][1]=square(true,"white",R1); 
	piece * N1 = new knight(1,2); board[1][2]=square(true,"white",N1); 
	piece * B1 = new bishop(1,3); board[1][3]=square(true,"white",B1); 
	piece * Q1 = new queen(1,4); board[1][4]=square(true,"white",Q1); 
	piece * K1 = new king(1,5,false); board[1][5]=square(true,"white",K1); 
	piece * B2 = new bishop(1,6); board[1][6]=square(true,"white",B2); 
	piece * N2 = new knight(1,7); board[1][7]=square(true,"white",N2); 
	piece * R2 = new rook(1,8,false); board[1][8]=square(true,"white",R2);
	
	piece * p1 = new pawn(7,1,false); board[7][1]=square(true,"black",p1);
	piece * p2 = new pawn(7,2,false); board[7][2]=square(true,"black",p2);
	piece * p3 = new pawn(7,3,false); board[7][3]=square(true,"black",p3);
	piece * p4 = new pawn(7,4,false); board[7][4]=square(true,"black",p4);
	piece * p5 = new pawn(7,5,false); board[7][5]=square(true,"black",p5);
	piece * p6 = new pawn(7,6,false); board[7][6]=square(true,"black",p6);
	piece * p7 = new pawn(7,7,false); board[7][7]=square(true,"black",p7);
	piece * p8 = new pawn(7,8,false); board[7][8]=square(true,"black",p8); 
	
	piece * r1 = new rook(8,1,false); board[8][1]=square(true,"black",r1); 
	piece * n1 = new knight(8,2); board[8][2]=square(true,"black",n1); 
	piece * b1 = new bishop(8,3); board[8][3]=square(true,"black",b1);
	piece * q1 = new queen(8,4); board[8][4]=square(true,"black",q1); 
	piece * k1 = new king(8,5,false); board[8][5]=square(true,"black",k1); 
	piece * b2 = new bishop(8,6); board[8][6]=square(true,"black",b2); 
	piece * n2 = new knight(8,7); board[8][7]=square(true,"black",n2); 
	piece * r2 = new rook(8,8,false); board[8][8]=square(true,"black",r2);//creating and placing all the pieces
	
	return board;//returning the new board
}

/*************************************************************************

A function to change the properties of the board and pieces to be moved, handling some
of the conditions too compicated to include in the move member functions. It also was easiest
to run the function to see if the board is in a state of check after a move (making the move illegal if
the person in check is the one doing the moving), in which case the board vector is left
unchanged, and this is checked for in the functions that call this function (e.g maketurn() )

*************************************************************************/

vector<vector<square> > movingpieces(vector<vector<square> > tempboard, string turn, int frank, int ffile, int trank, int tfile) {

	string otherturn = changeturn(turn);
	
	vector<vector<square> > savedboard(9, vector<square> (9));
	
	int i, j;
	
	savedboard = tempboard;

	piece * moved = new piece;
		
	moved = tempboard[frank][ffile].getpiece();
	
	moved = moved->changepos(trank,tfile);//change piece's intrinsic position information
	
	for (i=1; i<9; i++){
			
		for (j=1; j<9; j++) {
				
		if (tempboard[i][j].isoccupied() == true && tempboard[i][j].getpiece()->myname() == "pawn") {
				
				pawn * EP = (pawn*)tempboard[i][j].getpiece();
				
				EP = new pawn(i,j,EP->getmovedyet(),false);
					
				tempboard[i][j] = square(true,tempboard[i][j].colourocc(),EP);
				
			}
		}
	}	//cycles through board, marking every pawn as not capturable en-passant (since en-passant only works the turn immediately after a pawn
		//has moved two squares adjacent to an opposite pawn.

	if (tempboard[frank][ffile].getpiece()->myname()=="pawn"){
	
		if (tempboard[frank][ffile].colourocc()=="white" && trank == 8){
				
			moved = new queen(trank,tfile);
			tempboard[trank][tfile] = square(true,turn,moved);
		}
		
		else if(tempboard[frank][ffile].colourocc()=="black" && trank == 1){
		
			moved = new queen(trank,tfile);
			tempboard[trank][tfile] = square(true,turn,moved);
		}//pawn promotion if pawn reaches the end of the board (1st rank for black, 8th for white) 
		
		else if (abs(frank-trank)==2){
		
			moved = new pawn(trank,tfile,true,true);
		
			tempboard[trank][tfile] = square(true,turn,moved);
			tempboard[frank][ffile]= square(false,"empty",0);
			
		}//marking pawns as capturable by en-passant if they move 2 squares
		
		else if (abs(trank-frank)==1 && abs(tfile-ffile) == 1 && tempboard[trank][tfile].isoccupied() == false){
		
			if (tempboard[frank][ffile].colourocc()=="white") tempboard[trank-1][tfile]=square(false,"empty",0);
			else tempboard[trank+1][tfile]=square(false,"empty",0);
			
			tempboard[trank][tfile] = square(true,turn,moved);
			tempboard[frank][ffile]= square(false,"empty",0);
		}//deleting the captured pawn from an enpassant capture
				
		else tempboard[trank][tfile] = square(true,turn,moved);//otherwise allowing any other legal pawn move without special rules attached to them
			
	}
		
	else if (tempboard[frank][ffile].getpiece()->myname() == "king" && abs(ffile-tfile)==2){
		
		piece * castled = new piece;
			
		if (tfile == 3 && tempboard[trank][1].isoccupied() == true){
			
			castled = tempboard[trank][1].getpiece();
			castled = castled->changepos(trank,4);
				
			tempboard[trank][1] = square(false,"empty",0);
			tempboard[trank][tfile] = square(true,turn,moved);
			tempboard[trank][4] = square(true,turn,castled);
					
				
		}//moving the rook in the case of queenside castling
				
		else if (tfile == 7 && tempboard[trank][1].isoccupied() == true){
			
			castled = tempboard[trank][8].getpiece();
			castled = castled->changepos(trank,6);
				
			tempboard[trank][8] = square(false,"empty",0);
			tempboard[trank][tfile] = square(true,turn,moved);
			tempboard[trank][6] = square(true,turn,castled);
		}//moving the rook for kingside castling
		
	}	
		
	else tempboard[trank][tfile] = square(true,turn,moved);//any other move
	
	tempboard[frank][ffile]= square(false,"empty",0);//clears the square vacated by the moved piece 
	
	if (check(otherturn, tempboard)==true) tempboard = savedboard;	//if the board ends up so that the person moving is in check
																	//then revert the board to how it entered the function
	
	return tempboard;
}

/******************************************************************************

Function to determine whether or not a player is in check. The logic behind the function is that
if any piece can legally take the opposite king, then that's check.

******************************************************************************/	      

bool check(string turn, vector<vector<square> > board){

	int i, j;
	
	piece kingloc;
	
	bool flag=false;
	
	for (i=1; i<9; i++) {
	
		for (j=1; j<9; j++) {
		
			if (board[i][j].isoccupied()==true){
		
				if (board[i][j].getpiece()->myname()=="king"
				&& turn != board[i][j].colourocc()){
				
					kingloc = piece(board[i][j].getpiece()->getrank(),
					board[i][j].getpiece()->getfile());
				}
									
			}
		}
	
	}//loops to find the opposite coloured king
	
	for (i=1; i<9; i++) {
	
		for (j=1; j<9; j++) {
		
			if (board[i][j].isoccupied()==true 
			&& board[i][j].colourocc()==turn
			&& board[i][j].getpiece()->move(kingloc,board,turn) == true)
			flag = true;
		}
	}//loops through all pieces to see if any can legally take the king
			
	return flag;//returns true if it is check, false otherwise
	
}

/**********************************************************************************

Simple function to verfiy that two boards are identical. Necessary for seeing if the movingpieces() function
returned the same board, indicating an illegal move

**********************************************************************************/

bool identicalboards(vector<vector<square> > board1, vector<vector<square> > board2){

	int i, j;

	for (i=1; i<9; i++) {
		
			for (j=1; j<9; j++) {
			
				if (board1[i][j].isoccupied()==true
				&& board2[i][j].isoccupied()==true
				&& board1[i][j].getpiece()->myname() != board2[i][j].getpiece()->myname()) 
				return false;//corresponding squares occupy different pieces, not identical
				
				else if (board1[i][j].colourocc() != board2[i][j].colourocc()) return false;
				//corresponding squares have different colours on them
				
				else if (board1[i][j].isoccupied() != board2[i][j].isoccupied()) return false;
				//squares are occupied on one board and not on the other
			}
		}
	
	return true;//otherwise board identical
}

/*************************************************************************************

Function that builds on the earlier check function, to see if any legal moves can be made in order to avoid the king being
taken next turn. The function does just that, cycling through every square, trying to move every piece it finds to every other square
and then looking to see if its check.

*************************************************************************************/

bool mate(vector<vector<square> > tempboard, string turn) {

	int h, i, j, k;
	
	string otherturn = changeturn(turn);
	
	bool flag = true;
	
	piece matecheck;
	
	vector<vector<square> > savedboard(9, vector<square> (9));
	
	for (h=1; h<9; h++) {
	
		for (i=1; i<9; i++) {
				
			if (tempboard[h][i].isoccupied() == true && tempboard[h][i].colourocc()==otherturn) {
					
				for (j=1; j<9; j++) {
	
					for (k=1; k<9; k++) {
							
						matecheck = piece(j,k);
							
						if (tempboard[h][i].getpiece()->move(matecheck,tempboard,otherturn)==true) {
								
							savedboard = tempboard;//saving a copy of the board so changes can be made
								
							tempboard = movingpieces(tempboard,otherturn,h,i,j,k);
										  	  	  	  	  	  
							if(check(turn,tempboard)==false && identicalboards(tempboard,savedboard) == false) flag = false;
									
							tempboard = savedboard;//reverting to how board was before
								
						}
					}
				}
			}
		}	   
	}
	
	return flag;
}	 
			






