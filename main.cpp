#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <string>
#include <sstream>
#include <iostream>
#include <cmath>

sf::Clock deltaClock;

// Projection of a onto b
sf::Vector2f vectorProjection(sf::Vector2f a, sf::Vector2f b) {
    // Calculate b unit vector
    float bLength =  hypot(b.x,b.y);

    sf::Vector2f unitB(b.x/bLength, b.y/bLength);
    return sf::Vector2f(a.x * b.x,  a.y * b.y);
}



void translateObject(sf::CircleShape* object, sf::Vector2f aVec){
    object->setPosition(object->getPosition() + aVec);
}


void bounceOffBounds(sf::CircleShape* object, sf::Vector2f* curVel) {
    float x = curVel->x;
    float y = curVel->y;

    curVel->y = -y;
}



void bounceOffWall(sf::CircleShape* object, sf::Vector2f* curVel) {
    float x = curVel->x;
    float y = curVel->y;
    curVel->x = -x;
    curVel->y = y;
}

void bounceOffPlayer(sf::CircleShape* object, sf::Vector2f* curVel, float dif) {
    float x = curVel->x;
    float y = curVel->y;

    // Ball hits lower racket
    if(dif<0) {
        curVel->y = sf::Vector2f(x, 20+(-dif)).y;
    }

    // Ball hits upper racket
    if(dif>0) {
        curVel->y = sf::Vector2f(x, 20+(-dif)).y;
    }
    // Ball hits middle of racket
    if(dif=0) {
        curVel->y = -y;
    }


    curVel->x = -x;
}

void centerOrigin(sf::RectangleShape* shape){
    sf::Vector2f dim = shape->getSize();
    shape->setOrigin(dim.x/2, dim.y/2);
}


void centerOrigin(sf::CircleShape* shape){
    float rad = shape->getRadius();
    shape->setOrigin(rad, rad);
}



int main()
{
    sf::VideoMode vMode(500,500);
    sf::RenderWindow window(vMode, "PONG");

    sf::Font font;
    if(!font.loadFromFile("bahnschrift.ttf")){
        std::cout  << "Could not load font bahnschrift.ttf." << std::endl;
    }
    sf::Text fpsCounter;
    fpsCounter.setFont(font);
    fpsCounter.setCharacterSize(16);



    // Wall
    sf::RectangleShape wall(sf::Vector2f(10, window.getSize().y));
    wall.setPosition(window.getSize().x-10,0);
    wall.setFillColor(sf::Color::White);


    // Player
    sf::RectangleShape player(sf::Vector2f(40.0,160.0));
    player.setFillColor(sf::Color::Black);
    player.setOutlineColor(sf::Color::White);
    player.setOutlineThickness(1.f);
    centerOrigin(&player);
    player.setPosition(45.0f,window.getSize().y/2);
    bool inPlay = false;



    // Bounds
    sf::Vector2f boundDimension(window.getSize().x,3);
    sf::RectangleShape upperBound(boundDimension);
    sf::RectangleShape lowerBound(boundDimension);
    upperBound.setPosition(0,0);
    lowerBound.setPosition(0, window.getSize().y-3);

    upperBound.setFillColor(sf::Color::Green);
    lowerBound.setFillColor(sf::Color::Green);

    // Ball
    sf::CircleShape ball(25.f, 20);
    ball.setFillColor(sf::Color::White);
    centerOrigin(&ball);


    float speed = 7000.f;



    // Used to handle events
    sf::Event event;
    

    // User mouse
    sf::Mouse mouse;

    sf::FloatRect wBounds = wall.getGlobalBounds();


    sf::Vector2f fireVelo(200,0);
    sf::Rect<float> intersection;

    while (window.isOpen())
    {
        window.clear();
        sf::FloatRect bBounds = ball.getGlobalBounds();
        sf::FloatRect pBounds = player.getGlobalBounds();        
        sf::FloatRect uBounds = upperBound.getGlobalBounds();        
        sf::FloatRect lBounds = lowerBound.getGlobalBounds();        
        float dt = deltaClock.restart().asSeconds();

        if(!inPlay) {
            ball.setPosition(player.getPosition().x+20+ball.getRadius(),player.getPosition().y);
        }



        while (window.pollEvent(event))
        {
			if (event.type == sf::Event::Resized)
			{
				sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
				window.setView(sf::View(visibleArea));
			}

			if (event.type == sf::Event::Closed)
				window.close();
        
        


            // Key mapping [(w,22),(a,0),(s,18),(d,3)]
            // Space = 57

            if (event.type == sf::Event::KeyPressed)
			{
				std::cout << event.key.code << std::endl;
				// std::cout << 1.f/dt << std::endl; <- fps
                if(event.key.code == 22 && !pBounds.intersects(uBounds)) {
                    player.setPosition(player.getPosition().x,player.getPosition().y - (speed * dt));
                }
                if(event.key.code == 18 && !pBounds.intersects(lBounds)) {
                    player.setPosition(player.getPosition().x,player.getPosition().y + (speed * dt));
                }


                // FIRE!!!
                if(event.key.code == 57 & !inPlay) {
                    inPlay = true;
                }

			}



            if (event.type == sf::Event::MouseButtonPressed)
            {
                sf::Vector2f mousePosition(mouse.getPosition(window));
                std::cout << mousePosition.x << std::endl;
                std::cout << mousePosition.y << std::endl;
                sf::Vector2f ballPosition(ball.getPosition());
                ball.setPosition(mousePosition);
            }
        }

        if(bBounds.intersects(wBounds)) {
            bounceOffWall(&ball, &fireVelo);
        }
        if(bBounds.intersects(uBounds)) {
            bounceOffBounds(&ball, &fireVelo);
        }
        if(bBounds.intersects(lBounds)) {
            bounceOffBounds(&ball, &fireVelo);
        }
        
        if(bBounds.intersects(pBounds, intersection)) {
            bounceOffPlayer(&ball, &fireVelo, player.getPosition().y-ball.getPosition().y);
        }



        if(inPlay){
            translateObject(&ball, fireVelo * dt);
        }

        window.draw(wall);
        window.draw(player);
        window.draw(ball);
        window.draw(upperBound);
        window.draw(lowerBound);


        window.display();
    }

    return 0;
}