# include "Label.h"
# include <cstdlib>
# include <iostream>
# include "machine.h"

using namespace std;

unsigned Label::_counter = 0;

Label::Label() {
    _number = _counter++;
}

ostream &operator <<(ostream &ostr, const Label &label) {
    return ostr << label_prefix << label.number();
}