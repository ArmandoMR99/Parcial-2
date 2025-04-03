using System;
using System.Collections.Generic;
using NUnit.Framework;

public abstract class NodoBT
{
    public abstract bool Ejecutar();
}

// Root solo tiene un único child y no puede ser otro Root 
public class NodoRaiz : NodoBT
{
    private NodoBT hijo;
    public NodoRaiz(NodoBT hijo)
    {
        if (hijo is NodoRaiz)
            throw new ArgumentException("Un NodoRaiz no puede tener otro NodoRaiz como hijo.");
        this.hijo = hijo;
    }
    public override bool Ejecutar() => hijo?.Ejecutar() ?? false;
}

// Un nodo Composite no puede ser instanciado directamente
public abstract class NodoCompuesto : NodoBT
{
    protected List<NodoBT> hijos = new List<NodoBT>();
    public void AgregarHijo(NodoBT hijo)
    {
        if (hijo is NodoRaiz)
            throw new ArgumentException("Un NodoCompuesto no puede tener un NodoRaiz como hijo.");
        hijos.Add(hijo);
    }
}

public class NodoSelector : NodoCompuesto
{
    public override bool Ejecutar()
    {
        foreach (var hijo in hijos)
            if (hijo.Ejecutar()) return true;
        return false;
    }
}

public class NodoSecuencia : NodoCompuesto
{
    public override bool Ejecutar()
    {
        foreach (var hijo in hijos)
            if (!hijo.Ejecutar()) return false;
        return true;
    }
}

// Un nodo Task no puede ser instanciado directamente y no tiene hijos
public abstract class NodoTarea : NodoBT { }

public class NodoVerificarPar : NodoTarea
{
    private int numero;
    public NodoVerificarPar(int num) => numero = num;
    public override bool Ejecutar() => numero % 2 == 0;
}

// Un árbol de comportamiento solo tiene un NodoRaiz
public class BehaviourTree
{
    public NodoRaiz Root { get; private set; }
    public BehaviourTree(NodoRaiz root)
    {
        Root = root ?? throw new ArgumentException("El árbol de comportamiento debe tener un NodoRaiz.");
    }
    public bool Ejecutar() => Root.Ejecutar();
}

// Pruebas mínimas para garantizar los requerimientos
[TestFixture]
public class BehaviourTreeTests
{
    [Test] public void Root_TieneUnUnicoHijo() => Assert.Throws<ArgumentException>(() => new NodoRaiz(new NodoRaiz(new NodoVerificarPar(2))));
    [Test] public void NodoCompuesto_NoPuedeTenerRootComoHijo() => Assert.Throws<ArgumentException>(() => new NodoSelector().AgregarHijo(new NodoRaiz(new NodoVerificarPar(2))));
    [Test] public void RootVacio_RetornaFalse() => Assert.IsFalse(new BehaviourTree(new NodoRaiz(null)).Ejecutar());
    [Test]
    public void SequenceConFalse_RetornaFalse()
    {
        var seq = new NodoSecuencia();
        seq.AgregarHijo(new NodoVerificarPar(3)); 
        Assert.IsFalse(seq.Ejecutar());
    }
    [Test]
    public void SelectorConTrue_RetornaTrue()
    {
        var sel = new NodoSelector();
        sel.AgregarHijo(new NodoVerificarPar(2)); 
        Assert.IsTrue(sel.Ejecutar());
    }
}


// Posdata no he dormido por este parcial, :) gracias profe
