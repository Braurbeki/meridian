#pragma once
#include "project/Clip.h"

#include <string>
#include <vector>

namespace mer::project {

/// A folder of clips. Bins nest, but only one level deep in practice.
class Bin {
public:
    Bin() = default;
    explicit Bin(std::string name) : name_(std::move(name)) {}

    const util::Uuid&  id() const { return id_; }
    void               setId(const util::Uuid& id) { id_ = id; }
    const std::string& name() const { return name_; }
    void               setName(std::string n) { name_ = std::move(n); }

    void add(ClipPtr clip);
    bool remove(const util::Uuid& clipId);

    const std::vector<ClipPtr>& clips() const { return clips_; }
    ClipPtr                     find(const util::Uuid& clipId) const;
    std::size_t                 size() const { return clips_.size(); }

    const std::vector<Bin>& children() const { return children_; }
    void                    addChild(Bin b) { children_.push_back(std::move(b)); }

private:
    util::Uuid           id_ = util::Uuid::generate();
    std::string          name_;
    std::vector<ClipPtr> clips_;
    std::vector<Bin>     children_;
};

} // namespace mer::project
