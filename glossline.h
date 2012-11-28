#ifndef GLOSSLINE_H
#define GLOSSLINE_H

class WritingSystem;

class GlossLine {
public:
    enum LineType {
        Text,
        Gloss
    };

    GlossLine();
    GlossLine(LineType type, WritingSystem *ws);

    void setType(LineType type);
    void setWritingSystem(WritingSystem *ws);
    LineType type() const;
    WritingSystem* writingSystem() const;

private:
    LineType mType;
    WritingSystem *mWritingSystem;
};


#endif // GLOSSLINE_H
