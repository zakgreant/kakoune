#ifndef insert_completer_hh_INCLUDED
#define insert_completer_hh_INCLUDED

#include "buffer.hh"
#include "option_manager.hh"

#include "optional.hh"

namespace Kakoune
{

struct InsertCompleterDesc
{
    enum Mode
    {
        Word,
        Option,
        Filename
    };

    InsertCompleterDesc(Mode mode = Filename,
                        Optional<String> param = Optional<String>{})
        : mode{mode}, param{std::move(param)}
    {}

    bool operator==(const InsertCompleterDesc& other) const
    { return mode == other.mode and param == other.param; }

    bool operator!=(const InsertCompleterDesc& other) const
    { return !(*this == other); }

    Mode mode;
    Optional<String> param;
};

using InsertCompleterDescList = Vector<InsertCompleterDesc, MemoryDomain::Options>;


String option_to_string(const InsertCompleterDesc& opt);
void option_from_string(StringView str, InsertCompleterDesc& opt);

using ComplAndDesc = std::pair<String, String>;
using ComplAndDescList = Vector<ComplAndDesc>;

struct InsertCompletion
{
    ByteCoord begin;
    ByteCoord end;
    ComplAndDescList candidates;
    size_t timestamp;

    bool is_valid() const { return not candidates.empty(); }
};

class InsertCompleter : public OptionManagerWatcher
{
public:
    InsertCompleter(const Context& context);
    InsertCompleter(const InsertCompleter&) = delete;
    InsertCompleter& operator=(const InsertCompleter&) = delete;
    ~InsertCompleter();

    void select(int offset, Vector<Key>& keystrokes);
    void update();
    void reset();

    void explicit_file_complete();
    void explicit_word_complete();
    void explicit_line_complete();

private:
    bool setup_ifn();

    template<typename CompleteFunc>
    bool try_complete(CompleteFunc complete_func);
    void on_option_changed(const Option& opt) override;

    void menu_show();

    const Context&   m_context;
    OptionManager&   m_options;
    InsertCompletion m_completions;
    ComplAndDescList m_matching_candidates;
    int              m_current_candidate = -1;
};

}

#endif // insert_completer_hh_INCLUDED
