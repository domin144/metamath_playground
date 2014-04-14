#ifndef PROOF_STEP_H
#define PROOF_STEP_H

class Proof_step;
class Assertion;
class Essential_hypothesis;
class Floating_hypothesis;
class Assertion_step;
class Essential_hypothesis_step;
class Floating_hypothesis_step;
class Named_statement;
class Add_reference_step;
class Refer_step;
class Unknown_step;
//------------------------------------------------------------------------------
class Const_proof_step_visitor
{
public:
    virtual void operator()( const Assertion_step *step ) = 0;
    virtual void operator()( const Essential_hypothesis_step *step ) = 0;
    virtual void operator()( const Floating_hypothesis_step *step ) = 0;
    virtual void operator()( const Add_reference_step *step ) = 0;
    virtual void operator()( const Refer_step *step ) = 0;
    virtual void operator()( const Unknown_step *step ) = 0;
};
//------------------------------------------------------------------------------
class Proof_step
{
public:
    virtual void accept( Const_proof_step_visitor &visitor ) const = 0;
    virtual ~Proof_step()
    { }
};
//------------------------------------------------------------------------------
class Assertion_step : public Proof_step
{
public:
    Assertion_step( const Assertion *assertion ) :
        m_assertion( assertion )
    { }
    virtual void accept( Const_proof_step_visitor &visitor ) const override
    {
        visitor( this );
    }
    const Assertion *get_assertion() const
    {
        return m_assertion;
    }
private:
    const Assertion *m_assertion;
};
//------------------------------------------------------------------------------
class Essential_hypothesis_step : public Proof_step
{
public:
    Essential_hypothesis_step( const Essential_hypothesis *hypothesis ) :
        m_hypothesis( hypothesis )
    { }
    virtual void accept( Const_proof_step_visitor &visitor ) const override
    {
        visitor( this );
    }
    const Essential_hypothesis *get_hypothesis() const
    {
        return m_hypothesis;
    }
private:
    const Essential_hypothesis *m_hypothesis;
};
//------------------------------------------------------------------------------
class Floating_hypothesis_step : public Proof_step
{
public:
    Floating_hypothesis_step( const Floating_hypothesis *hypothesis ) :
        m_hypothesis( hypothesis )
    { }
    virtual void accept( Const_proof_step_visitor &visitor ) const override
    {
        visitor( this );
    }
    const Floating_hypothesis *get_hypothesis() const
    {
        return m_hypothesis;
    }
private:
    const Floating_hypothesis *m_hypothesis;
};
//------------------------------------------------------------------------------
class Add_reference_step : public Proof_step
{
public:
    virtual void accept( Const_proof_step_visitor &visitor ) const override
    {
        visitor( this );
    }
};
//------------------------------------------------------------------------------
class Refer_step : public Proof_step
{
public:
    Refer_step( int index ) :
        m_index( index )
    { }
    virtual void accept( Const_proof_step_visitor &visitor ) const override
    {
        visitor( this );
    }
    int get_index() const
    {
        return m_index;
    }
private:
    // index in the vector of referred expression, which does not include
    // referred statements listed in brackets at start of compressed proof
    int m_index;
};
//------------------------------------------------------------------------------
class Unknown_step : public Proof_step
{
public:
    virtual void accept( Const_proof_step_visitor &visitor ) const override
    {
        visitor( this );
    }
};

#endif // PROOF_STEP_H
