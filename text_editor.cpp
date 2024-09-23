#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <vector>
#include <string>
#include <fstream>

class terminalSettings
{
public:
    terminalSettings()
    {
        std::cout << "\033[2J";
        std::cout << "\033[1;1H";
        tcgetattr(STDIN_FILENO, &orig_termios);
        newtermios = orig_termios;
        newtermios.c_lflag &= ~(ICANON | ECHO | ECHOE | ECHOK | ECHONL | ISIG);
        newtermios.c_lflag &= ~(OPOST);
        newtermios.c_lflag &= ~(ICRNL | IXON | INPCK | ISTRIP);
        newtermios.c_iflag &= ~(IXON);
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
    }

    ~terminalSettings()
    {
        std::cout << "\033[2J";
        std::cout << "\033[1;1H";
        tcsetattr(STDIN_FILENO, TCSANOW, &orig_termios);
    }

    char getChar()
    {
        char c;
        read(STDIN_FILENO, &c, 1);
        return c;
    }

    void enableecho()
    {
        newtermios.c_lflag |= ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
    }

    void disableecho()
    {
        newtermios.c_lflag &= ~ECHO;
        tcsetattr(STDIN_FILENO, TCSANOW, &newtermios);
    }

    void movecursorup(int n = 1)
    {
        std::cout << "\033[" << n << "A";
    }

    void movecursordown(int n = 1)
    {
        std::cout << "\033[" << n << "B";
    }

    void movecursorleft(int n = 1)
    {
        std::cout << "\033[" << n << "D";
    }

    void movecursorright(int n = 1)
    {
        std::cout << "\033[" << n << "C";
    }

private:
    struct termios newtermios, orig_termios;
};
void saveFile(std::string filename, std::vector<std::string>data)
    {
        std::cout << std::endl;
        std::cout << "Enter the file name:- ";
        std::cin >> filename;
        std::ofstream saveFile(filename);
        if (saveFile.is_open())
        {
            for (const auto &line : data)
            {
                saveFile << line << std::endl;
            }
            saveFile.flush();
            saveFile.close();
            std::cout << filename << " saved successfully" << std::endl;
        }
        else
        {
            std::cerr << "Failed to save file" << std::endl;
        }
    }
int main()
{
    terminalSettings input;
    char c;
    std::cout << "Press any key (OR Press Q to exit)" << std::endl;
    std::string currentline;
    std::vector<std::string> data;
    bool isEditorMode = false;
    std::string filename;

    while (true)
    {
        c = input.getChar();
        if (isEditorMode)
        {
            if (c == 27)
            {
                c = input.getChar();
                if (c == 'i')
                {
                    std::cout << std::endl;
                    std::cout << "User is in editor mode" << std::endl;
                }
                else
                {
                    isEditorMode = false;
                    input.disableecho();
                    std::cout << "User is in control mode" << std::endl;
                }
            }
            else if (c == '\n')
            {
                data.push_back(currentline);
                currentline.clear();
                std::cout << std::endl;
            }
            else if (c == 19)
            {
                saveFile(filename, data);
            }
            else if (c == 127 || c == 8)
            {
                if (!currentline.empty())
                {
                    currentline.pop_back();
                    std::cout << "\b \b";
                }
            }
            else
            {
                currentline += c;
            }
        }
        else
        {
            if (c == 27)
            {
                c = input.getChar();
                if (c == 'i')
                {
                    isEditorMode = true;
                    input.enableecho();
                    std::cout << "Switched to editor mode. Type your text." << std::endl;
                }
            }
            else
            {
                if (c == 'q' || c == 'Q')
                    break;
                if (c == 'w' || c == 'W')
                    input.movecursorup();
                else if (c == 'a' || c == 'A')
                    input.movecursorleft();
                else if (c == 's' || c == 'S')
                    input.movecursordown();
                else if (c == 'd' || c == 'D')
                    input.movecursorright();
                else
                    std::cout << "Unknown command: " << c << std::endl;
            }
        }
    }
    return 0;
}
