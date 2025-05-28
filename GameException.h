#ifndef GAME_EXCEPTIONS_H
#define GAME_EXCEPTIONS_H

#include <exception>
#include <string>
#include <iostream>

class GameException : public std::exception {
protected:
    std::string message;
    std::string context;
    
public:
    GameException(const std::string& msg, const std::string& ctx = "") : message(msg), context(ctx) {}
    
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
    
    const std::string& getContext() const { return context; }
    
    virtual void logError() const {
        std::cerr << "[EROARE] " << message;
        if (!context.empty()) {
            std::cerr << " (Context: " << context << ")";
        }
        std::cerr << std::endl;
    }
};

class EntityException : public GameException {
public:
    EntityException(const std::string& msg, const std::string& entityType = "") : GameException("Entity Error: " + msg, entityType) {}
    
    void logError() const override {
        std::cerr << "[ENTITY ERROR] " << message;
        if (!context.empty()) {
            std::cerr << " (Entity: " << context << ")";
        }
        std::cerr << std::endl;
    }
};

class InvalidPositionException : public EntityException {
public:
    InvalidPositionException(float x, float y, const std::string& entityType = "") : EntityException("Invalid position (" + std::to_string(x) + ", " + std::to_string(y) + ")", entityType) {}
};

class DeadEntityException : public EntityException {
public:
    DeadEntityException(const std::string& action, const std::string& entityType = "") : EntityException("Attempted " + action + " on dead entity", entityType) {}
};

class GameStateException : public GameException {
public:
    GameStateException(const std::string& msg, const std::string& state = "") : GameException("Game State Error: " + msg, state) {}
    
    void logError() const override {
        std::cerr << "[GAME STATE ERROR] " << message;
        if (!context.empty()) {
            std::cerr << " (State: " << context << ")";
        }
        std::cerr << std::endl;
    }
};

class InvalidWaveException : public GameStateException {
public:
    InvalidWaveException(int wave) : GameStateException("Invalid wave number: " + std::to_string(wave), "Wave Management") {}
};

class BossNotAvailableException : public GameStateException {
public:
    BossNotAvailableException() : GameStateException("Boss action attempted when boss not available", "Boss Fight") {}
};

class ResourceException : public GameException {
public:
    ResourceException(const std::string& msg, const std::string& resource = "") : GameException("Resource Error: " + msg, resource) {}
    
    void logError() const override {
        std::cerr << "[RESOURCE ERROR] " << message;
        if (!context.empty()) {
            std::cerr << " (Resource: " << context << ")";
        }
        std::cerr << std::endl;
    }
};

class PowerupException : public ResourceException {
public:
    PowerupException(const std::string& powerupType, const std::string& msg) : ResourceException("Powerup error with " + powerupType + ": " + msg, powerupType) {}
};

class InputException : public GameException {
public:
    InputException(const std::string& msg, const std::string& inputType = "") : GameException("Input Error: " + msg, inputType) {}
    
    void logError() const override {
        std::cerr << "[INPUT ERROR] " << message;
        if (!context.empty()) {
            std::cerr << " (Input: " << context << ")";
        }
        std::cerr << std::endl;
    }
};

class InvalidCommandException : public InputException {
public:
    InvalidCommandException(char command) : InputException("Invalid command: " + std::string(1, command), "Command Input") {}
};

class CombatException : public GameException {
public:
    CombatException(const std::string& msg, const std::string& combatContext = "") : GameException("Combat Error: " + msg, combatContext) {}
    
    void logError() const override {
        std::cerr << "[COMBAT ERROR] " << message;
        if (!context.empty()) {
            std::cerr << " (Combat: " << context << ")";
        }
        std::cerr << std::endl;
    }
};

class WeaponException : public CombatException {
public:
    WeaponException(const std::string& weaponType, const std::string& msg) : CombatException("Weapon error with " + weaponType + ": " + msg, weaponType) {}
};

class ProjectileException : public CombatException {
public:
    ProjectileException(const std::string& msg) : CombatException("Projectile error: " + msg, "Projectile System") {}
};

class CriticalGameException : public GameException {
public:
    CriticalGameException(const std::string& msg, const std::string& system = "") : GameException("CRITICAL ERROR: " + msg, system) {}
    
    void logError() const override {
        std::cerr << "\n!!! CRITICAL GAME ERROR !!!" << std::endl;
        std::cerr << message;
        if (!context.empty()) {
            std::cerr << " (System: " << context << ")";
        }
        std::cerr << "\nGame may need to terminate!" << std::endl;
    }
};

class MemoryException : public CriticalGameException {
public:
    MemoryException(const std::string& operation) : CriticalGameException("Memory allocation failed during: " + operation, "Memory Management") {}
};

class SystemException : public CriticalGameException {
public:
    SystemException(const std::string& system, const std::string& msg) : CriticalGameException("System failure in " + system + ": " + msg, system) {}
};

class ExceptionHandler {
public:
    static bool handleException(const GameException& e, bool continueGame = true) {
        e.logError();
        
        if (dynamic_cast<const CriticalGameException*>(&e)) {
            std::cerr << "Critical error detected. Game will attempt to continue but may be unstable." << std::endl;
            return false; 
        }
        
        return continueGame;
    }
    
    static bool handleUnknownException() {
        std::cerr << "[UNKNOWN ERROR] An unexpected error occurred in the game!" << std::endl;
        std::cerr << "Game will attempt to continue..." << std::endl;
        return true; 
    }
    
    static void logRecoveryAction(const std::string& action) {
        std::cout << "[RECOVERY] " << action << std::endl;
    }
};

#define SAFE_EXECUTE(code, errorMsg) \
    try { \
        code; \
    } catch (const GameException& e) { \
        if (!ExceptionHandler::handleException(e)) { \
            throw; \
        } \
    } catch (const std::exception& e) { \
        std::cerr << "[STD ERROR] " << errorMsg << ": " << e.what() << std::endl; \
    } catch (...) { \
        ExceptionHandler::handleUnknownException(); \
    }

#endif