#include <array>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
//#include <iostream>
#include <cmath>

//fix: minor fisheye (esp at high fov), collision when reversing
//add: textures, map loading/gen, up/down, gameplay, gamify

int main() {
    constexpr int screenW=1600;
    constexpr int screenH=800;

    sf::RenderWindow window(sf::VideoMode(screenW, screenH), "Ray-Caster", sf::Style::Titlebar | sf::Style::Close);

    //colours
    sf::Color grey(180,180,180);
    sf::Color green(0,255,0);

    // //map
    // constexpr int row=8;
    // constexpr int column=8;
    // int map[row][column] = {
    //     {1,1,1,1,1,1,1,1},
    //     {1,0,0,0,0,0,0,1},
    //     {1,0,0,0,0,1,0,1},
    //     {1,1,1,0,0,0,0,1},
    //     {1,0,0,0,0,1,0,1},
    //     {1,0,0,0,0,1,0,1},
    //     {1,0,0,0,0,1,0,1},
    //     {1,1,1,1,1,1,1,1},
    // };

    // constexpr int row=4; //hor
    // constexpr int column=8; //ver
    // int map[row][column] = {
    //     {1,1,1,1,1,1,1,1},
    //     {1,0,0,0,0,0,0,1},
    //     {1,0,0,0,0,1,0,1},
    //     {1,1,1,1,1,1,1,1},
    //     // {1,0,0,0,0,1,0,1,1},
    //     // {1,0,0,0,0,1,0,1,1},
    //     // {1,0,0,0,0,1,0,1,1},
    //     // {1,1,1,1,1,1,1,1,1},
    // };

    constexpr int row=10;
    constexpr int column=10;
    int map[row][column] = {
        {1,1,1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,0,0,0,1},
        {1,1,1,0,0,0,0,0,1,1},
        {1,0,0,0,0,0,0,0,0,1},
        {1,0,0,0,0,0,0,1,1,1},
        {1,0,0,0,0,1,0,0,0,1},
        {1,1,0,1,1,1,1,0,1,1},
        {1,0,0,0,0,1,0,0,0,1},
        {1,1,1,1,1,1,1,1,1,1},
    };

    //grid
    int numPoints=2*row+2*column+2;
    sf::VertexArray grid(sf::Lines, numPoints);
    float gridX=0;
    float gridY=0;
    for (int i=0;i<2*column+2;i++) { //ver
        grid[i].position=sf::Vector2f(gridX,0);
        i++;
        grid[i].position=sf::Vector2f(gridX,screenH);
        gridX+=screenW/(2*column);
    }
    for (int i=2*column+2;i<numPoints;i++) { //hor
        grid[i].position=sf::Vector2f(0,gridY);
        i++;
        grid[i].position=sf::Vector2f(screenW/2,gridY);
        gridY+=screenH/row;
    }
    sf::RectangleShape tile;
    tile.setSize(sf::Vector2f(screenW/(2*column), screenH/row));
    tile.setFillColor(grey);

    //player
    sf::Vector3f p(screenW/4,screenH/2,0); //position and direction (x, y, theta in rad)
    sf::ConvexShape player;
    player.setPointCount(3);
    player.setPoint(0, sf::Vector2f(p.x,p.y)); //drawing as triangle
    player.setPoint(1, sf::Vector2f(p.x-20,p.y-5.36));
    player.setPoint(2, sf::Vector2f(p.x-20,p.y+5.36));
    player.setFillColor(sf::Color::Green);
    player.setOrigin(p.x,p.y);
    constexpr int speed=100; //modify this one
    int setSpeed; //used for logic

    //ray
    constexpr int fov=60; //field of view in degrees
    std::array<sf::VertexArray, fov> ray;
    for (int i=0;i<fov;i++) {
        ray[i]=sf::VertexArray(sf::PrimitiveType::Lines,2);
        ray[i][0].color=sf::Color::Red;
        ray[i][1].color=sf::Color::Red;
    }
    std::array<float, fov> rl;

    //wall rectangles
    std::array<sf::RectangleShape, fov> wall;

    //algorithm use
    std::array<int, fov> side; //used for logic and to select wall colour (1 is ver, 0 is hor)
    sf::Vector2f scaledPos; //scaled position
    sf::Vector2i truncatedPos; //truncated scaled position
    sf::Vector2f internalPos; //internal position in square
    sf::Vector2f n; //normalised direction vector (max length 1)
    sf::Vector2f distX; //x: dist to first ver intercept, y: dist between consecutive ver intercepts
    sf::Vector2f distY; //x: dist to first hor intercept, y: dist between consecutive hor intercepts
    sf::Vector2f unit; //x: 1 or -1 step for ver intercept, y: 1 or -1 step for hor intercept

    sf::Clock clock;

    //collision
    constexpr float tol=10; //px from wall where collision activates

    while (window.isOpen()) {

        //fps and time setup
        window.setFramerateLimit(60);
        sf::Time dt=clock.restart(); //delta time

        //commands to close
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type==sf::Event::Closed) window.close();
            if (event.type==sf::Event::KeyPressed&&event.key.code==sf::Keyboard::Escape) window.close();
        }

        //keyboard control setup
        p.z=player.getRotation()*M_PI/180;
        setSpeed=speed;

        //DDA algo. checks grid+1 in direction of next nearest intercept for wall
        for (int i=0;i<fov;i++) {
            n.x=cos(p.z+(-(fov/2)+i)*M_PI/180);
            n.y=sin(p.z+(-(fov/2)+i)*M_PI/180);
            if (n.y==0)n.y=0.00001; //avoiding division by zero

            scaledPos={column*p.x/(screenW/2),row*p.y/screenH};
            truncatedPos={static_cast<int>(scaledPos.x),static_cast<int>(scaledPos.y)};
            internalPos={scaledPos.x-static_cast<float>(truncatedPos.x),
            scaledPos.y-static_cast<float>(truncatedPos.y)};

            if (n.x>0) {
                distX.x=(1-internalPos.x)/n.x;
                unit.x=1;
                distX.y=1/n.x;
            } else {
                distX.x=internalPos.x/n.x;
                unit.x=-1;
                distX.y=1/n.x;
            }
            if (n.y>0) {
                distY.x=(1-internalPos.y)/n.y;
                unit.y=1;
                distY.y=1/n.y;
            } else {
                distY.x=internalPos.y/n.y;
                unit.y=-1;
                distY.y=1/n.y;
            }

            while (true) {
                if (abs(distX.x)<abs(distY.x)) { //true if distance to next ver intercept shorter
                    side[i]=1;
                    distX.x+=distX.y;
                    truncatedPos.x+=unit.x;
                } else {
                    side[i]=0;
                    distY.x+=distY.y;
                    truncatedPos.y+=unit.y;
                }
                if (map[truncatedPos.y][truncatedPos.x]!=0)break;
            }

            //ray calculation
            ray[i][0].position={p.x,p.y};
            if (side[i]==1) {
                ray[i][1].position={p.x+n.x*(screenW/2)*abs(distX.x-distX.y)/column,
                p.y+n.y*screenH*abs(distX.x-distX.y)/row};
            } else ray[i][1].position={p.x+n.x*(screenW/2)*abs(distY.x-distY.y)/column,
                p.y+n.y*screenH*abs(distY.x-distY.y)/row};
            ray[i][0].position={p.x,p.y};
            rl[i]=sqrt(pow((ray[i][1].position.x-ray[i][0].position.x),2)
            +pow((ray[i][1].position.y-ray[i][0].position.y),2));

            //collision detection
            if (i==(fov/2)&&rl[i]<tol)setSpeed=0;

            //wall setup
            if (side[i]==1)green.g-=30;
            green.g-=rl[i]*20/fov;
            wall[i].setFillColor(green);
            green.g=255;
            wall[i].setSize(sf::Vector2f((screenW/2)/fov, 30000/(rl[i]*cos(abs(-(fov/2)+i)*M_PI/180))));
            wall[i].setPosition(((screenW/2)+(screenW/(2*column))+i*((screenW/2)/fov)),(screenH/2));
            wall[i].setOrigin((screenW/(2*column)),wall[i].getSize().y/2);
        }

        //keyboard control
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) { //forwards
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {setSpeed*=3;} //zoomies
            p.x+=cos(p.z)*setSpeed*dt.asSeconds(); p.y+=sin(p.z)*setSpeed*dt.asSeconds();
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) { //backwards
            p.x-=cos(p.z)*setSpeed*dt.asSeconds(); p.y-=sin(p.z)*setSpeed*dt.asSeconds();
        }
        player.setPosition(p.x, p.y);
        setSpeed=speed;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) { //rotate ccw
            player.setRotation(player.getRotation()-2*setSpeed*dt.asSeconds());
        } else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) { //rotate cw
            player.setRotation(player.getRotation()+2*setSpeed*dt.asSeconds());}

        //rendering
        window.clear(sf::Color::Black);
        for(int i=0;i<row;i++) {
            for(int j=0;j<column;j++){
                if(map[i][j] == 1)
                {
                    tile.setPosition(j*screenW/(2*column),i*screenH/row);
                    window.draw(tile);
                }
            }
        }
        window.draw(grid);
        window.draw(player);
        for (int i=0;i<fov;i++) {
            window.draw(ray[i]);
            window.draw(wall[i]);
        }
        window.display();
        //std::cout<<(1/dt.asSeconds())<<'\n'; //fps
    }
    return 0;
}

