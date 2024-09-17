#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>

class UnbufferedInput
{
public:
    // Constructor
    UnbufferedInput()
    {
        // Clear the entire screen
        std::cout << "\033[2J";
        // Move cursor to the top-left position
        std::cout << "\033[1;5H";
        // get the current terminal settings and store into orig_termios
        tcgetattr(STDIN_FILENO, &orig_termios);
        // Now copy those values into new termios
        newtermios = orig_termios;
        // Manipulate those values.
        newtermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG);
        newtermios.c_lflag &= ~(OPOST);
        newtermios.c_lflag &= ~(ICRNL | IXON | INPCK | ISTRIP);
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
        drawverticallines('-');
    }
    // Destructor to make the terminal to original state.
    ~UnbufferedInput()
    {
        // Clears the screen
        std::cout << "\033[2J";
        // Move cursor to the top left position
        std::cout << "\033[1;1H";
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }
    // Getchar functiion to get the value and retun the value
    char getChar(){
        char c;
        // STDIN_FILE_NO:- reads the single character from the keyboard
        read(STDIN_FILENO, &c, 1);
        return c;
    }
        void movecursorup(int n=1){
            std::cout << "\033[" << n << "A";
        }
        void movecursordown(int n=1){
            std::cout << "\033[" << n << "B";
        }
        void movecursorleft(int n=1){
            std::cout << "\033[" << n << "D";
        }
        void movecursorright(int n=1){
            std::cout << "\033[" << n << "C";
        }

private:
    struct termios newtermios, orig_termios;

    void drawverticallines(char c){
        int row_size = 0;
        int col_size = 0;
        struct winsize w;
        if (ioctl(STDIN_FILENO, TIOCGWINSZ, &w) == -1){
            perror("Invalid window size");
            return;
        }
        else{
            row_size = w.ws_row;
            col_size = w.ws_col;
        }
        for (int i = 1; i < row_size; i++){
            std::cout << c << std::endl;
        }
        std::cout << "\033[1;2H";
    }
};
int main(){
    // Creates the object of the class
    UnbufferedInput input;
    char c;
    std::cout << "Press any key (OR Press Q to exit)" << std::endl;
    // Get the value from the user until the user press the q button to quit.
    while (true)
    {
        // Get input from the user
        c = input.getChar();
        if (c == 'q')
            break;
        if(c=='w')
        input.movecursorup();
        else if(c=='a')
        input.movecursorleft();
        else if(c=='s')
        input.movecursordown();
        else if(c=='d')
        input.movecursorright();
        else
        std::cout << "You Pressed Key " << c << std::endl;
    }
    return 0;
}