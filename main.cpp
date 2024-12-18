#include <array>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

int main()
{
    sf::RenderWindow window(sf::VideoMode(1200, 600), "Ray-Caster", sf::Style::Titlebar | sf::Style::Close);

    //colours
    sf::Color Grey(180, 180, 180);
    sf::Color DarkGreen(70, 200, 40);

    //making grid
    sf::VertexArray grid(sf::Lines, 40);
    float gridX = 0;
    float gridY = 0;
    //vertical lines
    for (int i = 0; i < 18; i++) {
        grid[i].position = sf::Vector2f(gridX, 0);
        i++;
        grid[i].position = sf::Vector2f(gridX, 600);
        gridX += 75;
    }
    //horizontal lines
    for (int i = 20; i < 40; i++) {
        grid[i].position = sf::Vector2f(0, gridY);
        i++;
        grid[i].position = sf::Vector2f(600, gridY);
        gridY += 75;
    }
    //tiles
    sf::RectangleShape tile;
    tile.setSize(sf::Vector2f(75, 75));
    tile.setFillColor(Grey);
    //map
    int map[8][8] = {
        {1,1,1,1,1,1,1,1},
        {1,0,0,0,0,0,0,1},
        {1,0,0,0,0,1,0,1},
        {1,1,1,0,0,0,0,1},
        {1,0,0,0,0,1,0,1},
        {1,0,0,0,0,1,0,1},
        {1,0,0,0,0,1,0,1},
        {1,1,1,1,1,1,1,1},
    };

    //player
    sf::Vector3f p(300.f,300.f,0.f); //position and direction (x, y, theta in rad)
    sf::ConvexShape player;
    player.setPointCount(3);
    player.setPoint(0, sf::Vector2f(p.x,p.y));
    player.setPoint(1, sf::Vector2f(p.x-20,p.y-5.36f));
    player.setPoint(2, sf::Vector2f(p.x-20,p.y+5.36f));
    player.setFillColor(sf::Color::Green);
    player.setOrigin(p.x,p.y);

    //ray
    std::array<sf::VertexArray, 50> ray = {
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2),
        sf::VertexArray(sf::PrimitiveType::Lines,2)};
    for (int i=0;i<50;i++) {
        ray[i][0].color=sf::Color::Red;
        ray[i][1].color=sf::Color::Red;
    }
    std::array<float, 50> rl{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //ray length

    //wall rectangles
    std::array<sf::RectangleShape, 50> wall;

    //algorithm use
    std::array<int, 50> side{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,}; //used for algo and to select wall colour

    sf::Vector2f scaledPos; //scaled position
    sf::Vector2i truncatedPos; //scaled position truncated to int
    sf::Vector2f internalPos; //scaled position - truncated position (position within current square)

    sf::Vector2f vDist; //distance from initial position to first vertical intercept
    sf::Vector2f hDist; //distance from initial position to first horizontal intercept
    sf::Vector2f vInt; //tracker for vertical intercepts
    sf::Vector2f hInt; //tracker for horizontal intercepts
    sf::Vector2f delta; //how much each intercept moves
    sf::Vector2f unit; //1 or -1

    float tempPZ{0};

    sf::Clock clock;

    while (window.isOpen())
    {
        window.setFramerateLimit(60);
        sf::Time dt = clock.restart(); //delta time

        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        //movement
        p.z=player.getRotation()*M_PI/180;

        //forwards
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        {
            p.x+=cos(p.z)*100*dt.asSeconds();
            p.y+=sin(p.z)*100*dt.asSeconds();
            player.setPosition(p.x, p.y);
        }
        //backwards
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        {
            p.x-=cos(p.z)*100*dt.asSeconds();
            p.y-=sin(p.z)*100*dt.asSeconds();
            player.setPosition(p.x, p.y);
        }
        //rotate ccw
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        {
            player.rotate(-3);
        }
        //rotate cw
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        {
            player.rotate(3);
        }
        for (int i=0;i<50;i++) {
            ray[i][0].position={p.x,p.y};
            //ray[i][0].position={static_cast<float>(p.x-sin(p.z)*(i-25)*2), static_cast<float>(p.y+cos(p.z)*(i-25)*2)};
        }

        //algorithm
        //checks all ver and hor intercepts with tracker rays from init pos, compares which hits first
        for (int i=0;i<50;i++) {
            side[i]=-1;
        }

        for (int i=0;i<50;i++) {
            tempPZ=p.z+(-25+i)*M_PI/180;
            if (tempPZ<0) tempPZ=2*M_PI+tempPZ;
            if (tempPZ>2*M_PI) tempPZ=tempPZ-2*M_PI;
            //scaledPos={8*(p.x-sin(p.z)*(i-25)*2)/600,8*(p.y+cos(p.z)*(i-25)*2)/600}; //scaled position
            scaledPos={8*p.x/600,8*p.y/600}; //scaled position
            truncatedPos={static_cast<int>(scaledPos.x),static_cast<int>(scaledPos.y)}; //scaled position cast to int (map use)
            internalPos={scaledPos.x-static_cast<float>(truncatedPos.x),scaledPos.y-static_cast<float>(truncatedPos.y)}; //internal position in square

            if (abs(tempPZ-0)<0.0001||abs(tempPZ-2*M_PI)<0.0001) { //0 and 360
                vDist.x=1-internalPos.x;
                vDist.y=0;

                delta.y=0;

                unit.x=1;
            } else if (abs(tempPZ-M_PI/2)<0.0001) { //90
                hDist.x=0;
                hDist.y=1-internalPos.y;

                delta.x=0;

                unit.y=1;
            } else if (abs(tempPZ-M_PI)<0.0001) { //180
                vDist.x=-internalPos.x;
                vDist.y=0;

                delta.y=0;

                unit.x=-1;
            } else if (abs(tempPZ-3*M_PI/2)<0.0001) { //270
                hDist.x=0;
                hDist.y=-internalPos.y;

                delta.x=0;

                unit.y=-1;
            } else if (tempPZ>M_PI/2&&tempPZ<M_PI) { //2nd quadrant cw
                vDist.x=-internalPos.x; //-ve
                vDist.y=-vDist.x*tan(M_PI-tempPZ); //+ve
                hDist.y=1-internalPos.y; //+ve
                hDist.x=-hDist.y*1/tan(M_PI-tempPZ); //-ve

                delta.x=-1.f/tan(M_PI-tempPZ);
                delta.y=tan(M_PI-tempPZ);

                unit.x=-1;
                unit.y=1;
            } else if (tempPZ>M_PI&&tempPZ<3*M_PI/2) { //3rd quadrant cw
                vDist.x=-internalPos.x; //-ve
                vDist.y=vDist.x*tan(tempPZ-M_PI); //-ve
                hDist.y=-internalPos.y; //-ve
                hDist.x=hDist.y*1/tan(tempPZ-M_PI); //-ve

                delta.x=-1.f/tan(tempPZ-M_PI);
                delta.y=-tan(tempPZ-M_PI);

                unit.x=-1;
                unit.y=-1;
            } else if (tempPZ>3*M_PI/2&&tempPZ<2*M_PI) { //4th quadrant cw
                vDist.x=1-internalPos.x; //+ve
                vDist.y=-vDist.x*tan(2*M_PI-tempPZ); //-ve
                hDist.y=-internalPos.y; //-ve
                hDist.x=-hDist.y*1/tan(2*M_PI-tempPZ); //+ve

                delta.x=1.f/tan(2*M_PI-tempPZ);
                delta.y=-tan(2*M_PI-tempPZ);

                unit.x=1;
                unit.y=-1;
            } else { //1st quadrant cw
                vDist.x=1-internalPos.x;
                vDist.y=vDist.x*tan(tempPZ);
                hDist.y=1-internalPos.y;
                hDist.x=hDist.y*1/tan(tempPZ);

                delta.x=1.f/tan(tempPZ);
                delta.y=tan(tempPZ);

                unit.x=1;
                unit.y=1;
            }

            if (abs(tempPZ-0)<0.0001||abs(tempPZ-M_PI)<0.0001||abs(tempPZ-2*M_PI)<0.0001) { //0, 180, and 360
                vInt.x=scaledPos.x+vDist.x;
                vInt.y=scaledPos.y+vDist.y;
                if (abs(tempPZ-M_PI)<0.0001) { //180 adjustment
                    vInt.x-=1;
                }
                while (true) {
                    if (map[static_cast<int>(vInt.x)][static_cast<int>(vInt.y)]!=0)break;
                    vInt.x+=unit.x;
                    vInt.y+=delta.y;
                }
                if (abs(tempPZ-M_PI)<0.0001) { //180 adjustment
                    vInt.x+=1;
                }
                vDist.x=vInt.x-scaledPos.x;
                vDist.y=vInt.y-scaledPos.y;
                side[i]=1;
            } else if (abs(tempPZ-M_PI/2)<0.0001||abs(tempPZ-3*M_PI/2)<0.0001) { //90 and 270
                hInt.x=scaledPos.x+hDist.x;
                hInt.y=scaledPos.y+hDist.y;
                if (abs(tempPZ-3*M_PI/2)<0.0001) { //270 adjustment
                    hInt.y-=1;
                }
                while (true) {
                    if (map[static_cast<int>(hInt.x)][static_cast<int>(hInt.y)]!=0)break;
                    hInt.x+=delta.x;
                    hInt.y+=unit.y;
                }
                if (abs(tempPZ-3*M_PI/2)<0.0001) { //270 adjustment
                    hInt.y+=1;
                }
                hDist.x=hInt.x-scaledPos.x;
                hDist.y=hInt.y-scaledPos.y;
                side[i]=0;
            } else {

                //first intercept positions
                vInt.x=scaledPos.x+vDist.x;
                vInt.y=scaledPos.y+vDist.y;
                hInt.x=scaledPos.x+hDist.x;
                hInt.y=scaledPos.y+hDist.y;

                if (tempPZ>M_PI/2&&tempPZ<M_PI) { //2nd quadrant adjustment
                    vInt.x-=1;
                } else if (tempPZ>M_PI&&tempPZ<3*M_PI/2) { //3rd quadrant adjustment
                    vInt.x-=1;
                    hInt.y-=1;
                } else if (tempPZ>3*M_PI/2&&tempPZ<2*M_PI) { //4th quadrant adjustment
                    hInt.y-=1;
                }

                //checking all ver and hor intercepts separately
                while (true) {
                    if (map[static_cast<int>(vInt.x)][static_cast<int>(vInt.y)]!=0)break;
                    vInt.x+=unit.x;
                    vInt.y+=delta.y;
                }
                while (true) {
                    if (map[static_cast<int>(hInt.x)][static_cast<int>(hInt.y)]!=0||hInt.x<0||hInt.x>7||hInt.y<0||hInt.y>7)break;
                    hInt.x+=delta.x;
                    hInt.y+=unit.y;
                }

                if (tempPZ>M_PI/2&&tempPZ<M_PI) {
                    vInt.x+=1;
                } else if (tempPZ>M_PI&&tempPZ<3*M_PI/2) {
                    vInt.x+=1;
                    hInt.y+=1;
                } else if (tempPZ>3*M_PI/2&&tempPZ<2*M_PI) {
                    hInt.y+=1;
                }

                //distance from position to intercept
                vDist.x=vInt.x-scaledPos.x;
                vDist.y=vInt.y-scaledPos.y;
                hDist.x=hInt.x-scaledPos.x;
                hDist.y=hInt.y-scaledPos.y;

                //checking which hits first
                sqrt(vDist.x*vDist.x+vDist.y*vDist.y)<sqrt(hDist.x*hDist.x+hDist.y*hDist.y) ? side[i]=1 : side[i]=0; //side=1 is ver
            }
            if (side[i]==0) {
                ray[i][1].position={600*hInt.x/8,600*hInt.y/8};
            } else ray[i][1].position={600*vInt.x/8,600*vInt.y/8};

            rl[i]=sqrt(pow((ray[i][1].position.x-ray[i][0].position.x),2)+pow((ray[i][1].position.y-ray[i][0].position.y),2));
        }

        for (int i=0;i<50;i++) {
            side[i]==1 ? wall[i].setFillColor(sf::Color::Green) : wall[i].setFillColor(DarkGreen);
            wall[i].setSize(sf::Vector2f(12, 30000/(rl[i]*cos(abs(-25+i)*M_PI/180))));
            wall[i].setPosition((637.5+i*12),300);
            wall[i].setOrigin(37.5,wall[i].getSize().y/2);
            //wall[i].setScale(1,(1/rl[i]));
        }

        window.clear(sf::Color::Black);
        for(int i=0;i<8;i++) {
            for(int j=0;j<8;j++){
                if(map[i][j] == 1)
                {
                    tile.setPosition(static_cast<float>(i)*75,static_cast<float>(j)*75);
                    window.draw(tile);
                }
            }
        }
        window.draw(grid);
        window.draw(player);
        for (int i=0;i<50;i++) {
            window.draw(ray[i]);
            window.draw(wall[i]);
        }
        window.display();
    }
    return 0;
}