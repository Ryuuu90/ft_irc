#include "bot.hpp"

Bot::Bot() {
    std::srand(std::time(NULL));

    jdmFacts.push_back("The Nissan Skyline GT-R was known as 'Godzilla' in the Australian car press.");
    jdmFacts.push_back("The Toyota Supra MK4 featured the legendary 2JZ-GTE engine.");
    jdmFacts.push_back("The Mazda RX-7 is powered by a unique rotary engine.");
    jdmFacts.push_back("The Honda NSX was co-developed with input from Ayrton Senna.");
    jdmFacts.push_back("The Mitsubishi Lancer Evolution was initially intended for rally racing.");
    jdmFacts.push_back("The Subaru Impreza WRX STI is famous for its success in the World Rally Championship.");
    jdmFacts.push_back("The Datsun 240Z was a huge success in the American market.");
    jdmFacts.push_back("The Nissan Silvia S15 is one of the most popular cars for drifting.");
    jdmFacts.push_back("The Toyota AE86, also known as the Hachiroku, is iconic in the drifting community.");
    jdmFacts.push_back("The Mazda MX-5 Miata is the best-selling two-seat convertible sports car in history.");
    jdmFacts.push_back("The Toyota Celica GT-Four was a successful rally car in the late 1980s and early 1990s.");
    jdmFacts.push_back("The Nissan 300ZX (Z32) was praised for its advanced technology and performance.");
    jdmFacts.push_back("The Honda Civic Type R is known for its high-revving VTEC engine and sporty handling.");
    jdmFacts.push_back("The Mitsubishi 3000GT (GTO) featured advanced technology such as all-wheel drive and active aerodynamics.");
    jdmFacts.push_back("The Lexus LFA, although not old, is a modern classic known for its high-revving V10 engine and exceptional engineering.");
    jdmFacts.push_back("The Mazda Cosmo was the first car to be powered by a twin-rotor rotary engine.");
    jdmFacts.push_back("The Nissan Fairlady Z (240Z/260Z/280Z) helped establish Nissan's reputation as a maker of affordable sports cars.");
    jdmFacts.push_back("The Honda S2000 is renowned for its high-revving F20C engine, which held the record for the highest specific output of any naturally aspirated production car engine.");
    jdmFacts.push_back("The Mitsubishi Starion is one of the original turbocharged sports cars from Japan.");
    jdmFacts.push_back("The Toyota Chaser was a mid-size car that was popular among car enthusiasts for its rear-wheel-drive layout and powerful turbocharged engines.");
    jdmFacts.push_back("The Nissan R32 GT-R introduced the ATTESA E-TS all-wheel-drive system and the Super-HICAS four-wheel steering system.");
    jdmFacts.push_back("The Toyota MR2 had a mid-engine, rear-wheel-drive layout, giving it exceptional handling characteristics.");
    jdmFacts.push_back("The Honda Integra Type R is often regarded as one of the best front-wheel-drive cars ever made.");
    jdmFacts.push_back("The Nissan 240SX (Silvia in Japan) was popular for its balance and drift capabilities.");
    jdmFacts.push_back("The Mitsubishi Eclipse GSX featured a turbocharged engine and all-wheel drive.");
    jdmFacts.push_back("The Toyota Soarer (Lexus SC in the US) offered luxury and performance, featuring advanced technology like active suspension.");
    jdmFacts.push_back("The Nissan Pulsar GTI-R was a homologation special built for Group A rallying, featuring a turbocharged engine and all-wheel drive.");
    jdmFacts.push_back("The Honda CR-X was known for its light weight and excellent fuel economy, along with sporty performance.");
    jdmFacts.push_back("The Mazda RX-3 was another rotary-powered car that enjoyed success both on the track and in the market.");
    jdmFacts.push_back("The Toyota Cressida shared its platform with the Supra and was a favorite for engine swaps.");
    jdmFacts.push_back("The Nissan Bluebird SSS was a popular sporty sedan in Japan and was known for its rally heritage.");
    jdmFacts.push_back("The Subaru Legacy GT featured a turbocharged engine and all-wheel drive, laying the groundwork for future performance models.");
    jdmFacts.push_back("The Mazda 323 GTX was one of the first affordable all-wheel-drive turbocharged hatchbacks.");
    jdmFacts.push_back("The Nissan Figaro, a retro-styled limited-production car, has become a cult classic.");
    jdmFacts.push_back("The Toyota Century is Japan's ultimate luxury sedan, used by dignitaries and featuring a V12 engine in later models.");
    jdmFacts.push_back("The Suzuki Cappuccino is a tiny, lightweight kei car sports car that is highly prized for its handling and charm.");
    jdmFacts.push_back("The Nissan Laurel was known for its comfortable ride and powerful engine options, making it a sleeper hit among enthusiasts.");

}

std::string Bot::getRandomFact() const
{
    int randomIndex = std::rand() % jdmFacts.size();
    return jdmFacts[randomIndex];
}
