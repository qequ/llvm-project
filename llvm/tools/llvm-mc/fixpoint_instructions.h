#ifndef FIXPOINT_INSTRUCTIONS_H
#define FIXPOINT_INSTRUCTIONS_H
#include "include/ConstantAbstractDomain.h"
#include "include/DisjointUnionAbstractDomain.h"
#include "include/FiniteAbstractDomain.h"
#include "include/MonotonicFixpointIterator.h"

#include "include/HashedAbstractEnvironment.h"
#include "include/HashedSetAbstractDomain.h"
#include "include/PatriciaTreeMapAbstractEnvironment.h"
#include "include/PatriciaTreeSet.h"

#include <functional>
#include <initializer_list>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "qequinstructions.h"
#include <boost/functional/hash.hpp>

namespace typeChecking
{
    using namespace sparta;

    class PointerClass
    {
    private:
        unsigned int indirections;

    public:
        PointerClass(unsigned int indirections = 1) : indirections(indirections) {}

        friend std::ostream &operator<<(std::ostream &os, const PointerClass &)
        {
            return os;
        };

        bool operator==(const PointerClass otherPointer) const
        {
            return indirections == otherPointer.indirections;
        };
    };

    using PointerDomain = ConstantAbstractDomain<PointerClass>;
    using NumberDomain = ConstantAbstractDomain<uint64_t>;
    using PointerNumberDomain =
        DisjointUnionAbstractDomain<PointerDomain, NumberDomain>;

    using AbstractEnvironment =
        HashedAbstractEnvironment<std::string, PointerNumberDomain>;

    enum class TypesOptions
    {
        NUMBER,
        POINTER
    };

    struct Mnemonic
    {
        // constructor set default
        Mnemonic(const std::string &src, const std::string &dest) : src(src), dest(dest) {}
        virtual ~Mnemonic() = default;

        virtual void analyze_mnemonic(AbstractEnvironment *hash)
        {
            if (hash->get(dest).equals(NumberDomain(0)))
            {
                // it's a number
                analyze_dest_number(hash);
            }
            else
            {
                // it's a pointer
                analyze_dest_pointer(hash);
            };
        }

        virtual void printMsg()
        {
            std::cout << "Mnemonic" << std::endl;
        }

    private:
        virtual void analyze_dest_number(AbstractEnvironment *hash) const {}
        virtual void analyze_dest_pointer(AbstractEnvironment *hash) const {}

    protected:
        const std::string src;
        const std::string dest;
    };

    class Nope : public Mnemonic
    {
    public:
        // constructor
        Nope() : Mnemonic("", "") {}

        void analyze_mnemonic(AbstractEnvironment *hash) override {}
    };

    class SetType : public Mnemonic
    {
    public:
        SetType(const std::string& reg, TypesOptions value) : Mnemonic("", reg), value(value){};

        void analyze_mnemonic(AbstractEnvironment *hash) override
        {
            switch (value)
            {
            case TypesOptions::NUMBER:
                handle_dest_number(hash);
                break;
            case TypesOptions::POINTER:
                handle_dest_pointer(hash);
                break;
            default:
                throw std::runtime_error("unreachable");
            }
        };

    private:
        const TypesOptions value;

        void handle_dest_number(AbstractEnvironment *hash) const
        {
            hash->set(dest, NumberDomain(0));
        };
        void handle_dest_pointer(AbstractEnvironment *hash) const
        {
            hash->set(dest, PointerDomain(PointerClass()));
        };
    };

    struct Mov : public Mnemonic
    {
        const bool srcMemOp;

        // call mnemonic constructor to set src and dest
        Mov(const std::string& src, const bool srcMemOp, const std::string& dest) : Mnemonic(src, dest), srcMemOp(srcMemOp) {}

        void analyze_dest_number(AbstractEnvironment *hash) const override
        {
            if (srcMemOp)
            {
                // trying to make indirection of a number - error
                throw std::runtime_error("type check error");
            }
            else
            {
                hash->set(dest, hash->get(src));
            }
        };

        void analyze_dest_pointer(AbstractEnvironment *hash) const override
        {
            if (srcMemOp)
            {
                // indirection on pointer makes a number
                hash->set(dest, NumberDomain(0));
            }
            else
            {
                // trying to make indirection of a number - error
                hash->set(dest, hash->get(src));
            }
        };
    };

    struct Add : public Mnemonic
    {
        using Mnemonic::Mnemonic;

        void printMsg() override
        {
            std::cout << "Add" << src << " " << dest << std::endl;
        };

        void analyze_dest_number(AbstractEnvironment *hash) const override
        {
            if (!hash->get(src).equals(NumberDomain(0)))
            {
                // src is pointer
                hash->set(dest, hash->get(src));
            }
        };
        void analyze_dest_pointer(AbstractEnvironment *hash) const override
        {
            if (!hash->get(src).equals(NumberDomain(0)))
            {
                throw std::runtime_error("type check error");
            }
        };
    };

    struct Sub : public Mnemonic
    {
        using Mnemonic::Mnemonic;

        void analyze_dest_number(AbstractEnvironment *hash) const override
        {
            if (!hash->get(src).equals(NumberDomain(0)))
            {
                // src is pointer
                throw std::runtime_error("type check error");
            }
        };
        void analyze_dest_pointer(AbstractEnvironment *hash) const override
        {
            if (!hash->get(src).equals(NumberDomain(0)))
            {
                hash->set(dest, NumberDomain(0));
            }
        };
    };

    struct Cmp : public Mnemonic
    {
        using Mnemonic::Mnemonic;
        // Cmp(std::string src, std::string dest) : Mnemonic(src, dest) {}

        void analyze_dest_number(AbstractEnvironment *hash) const override
        {
            if (!hash->get(src).equals(NumberDomain(0)))
            {
                throw std::runtime_error("type check error");
            }
        };
        void analyze_dest_pointer(AbstractEnvironment *hash) const override
        {
            if (hash->get(src).equals(NumberDomain(0)))
            {
                throw std::runtime_error("type check error");
            }
        };
    };

    struct Mul : public Mnemonic
    {
        Mul(const std::string& dest) : Mnemonic("", dest) {}

        void analyze_dest_pointer(AbstractEnvironment *hash) const override
        {
            throw std::runtime_error("type check error");
        }
    };

    struct Div : public Mnemonic
    {
        Div(const std::string& dest) : Mnemonic("", dest) {}

        void analyze_dest_pointer(AbstractEnvironment *hash) const override
        {
            throw std::runtime_error("type check error");
        };
    };

    class BasicBlock;

    struct Edge final
    {
        Edge(BasicBlock *source, BasicBlock *target)
            : source(source), target(target) {}

        BasicBlock *source;
        BasicBlock *target;
    };

    class BasicBlock final
    {
    public:
        BasicBlock() = default;

        void add(std::unique_ptr<Mnemonic> mnemonic)
        {
            m_mnemonics.push_back(std::move(mnemonic));
        }

        void add_successor(BasicBlock *successor)
        {
            m_edges.push_back(std::make_unique<Edge>(this, successor));
            auto *edge = m_edges.back().get();
            m_successors.push_back(edge);
            successor->m_predecessors.push_back(edge);
        }

        const std::vector<std::unique_ptr<Mnemonic>> &mnemonics() const
        {
            return m_mnemonics;
        }

    private:
        std::vector<std::unique_ptr<Mnemonic>> m_mnemonics;
        std::vector<std::unique_ptr<Edge>> m_edges;
        std::vector<Edge *> m_predecessors;
        std::vector<Edge *> m_successors;

        friend class ProgramInterface;
    };

    class Program final
    {
    public:
        Program() = default;

        BasicBlock *create_block()
        {
            m_basic_blocks.push_back(std::make_unique<BasicBlock>());
            return m_basic_blocks.back().get();
        }

        void set_entry(BasicBlock *entry) { m_entry = entry; }

        void set_exit(BasicBlock *exit) { m_exit = exit; }

    private:
        std::vector<std::unique_ptr<BasicBlock>> m_basic_blocks;
        BasicBlock *m_entry = nullptr;
        BasicBlock *m_exit = nullptr;

        friend class ProgramInterface;
    };

    class ProgramInterface
    {
    public:
        using Graph = Program;
        using NodeId = BasicBlock *;
        using EdgeId = Edge *;

        static NodeId entry(const Graph &graph) { return graph.m_entry; }
        static NodeId exit(const Graph &graph) { return graph.m_exit; }
        static std::vector<EdgeId> predecessors(const Graph &, const NodeId &node)
        {
            return node->m_predecessors;
        }
        static std::vector<EdgeId> successors(const Graph &, const NodeId &node)
        {
            return node->m_successors;
        }
        static NodeId source(const Graph &, const EdgeId &e) { return e->source; }
        static NodeId target(const Graph &, const EdgeId &e) { return e->target; }
    };

    template <template <typename GraphInterface, typename Domain, typename NodeHash>
              class FixpointIteratorBase>
    class FixpointEngine final : public FixpointIteratorBase<
                                     ProgramInterface,
                                     AbstractEnvironment,
                                     std::hash<typename ProgramInterface::NodeId>>
    {
    private:
        using Base =
            FixpointIteratorBase<ProgramInterface,
                                 AbstractEnvironment,
                                 std::hash<typename ProgramInterface::NodeId>>;
        using NodeId = typename Base::NodeId;
        using EdgeId = typename Base::EdgeId;

    public:
        explicit FixpointEngine(const Program &program) : Base(program) {}

        void analyze_node(const NodeId &bb,
                          AbstractEnvironment *current_state) const override
        {
            for (const auto &mnemonic : bb->mnemonics())
            {
                analyze_mnemonic(mnemonic.get(), current_state);
            }
        }

        void analyze_mnemonic(Mnemonic *mnemonic,
                              AbstractEnvironment *current_state) const
        {
            mnemonic->analyze_mnemonic(current_state);
        }

        AbstractEnvironment analyze_edge(
            const EdgeId &, const AbstractEnvironment &state) const override
        {
            return state;
        }
    };
}

#endif
